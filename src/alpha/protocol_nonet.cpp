// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

// proof of concept dummy impl of the alpha network layer

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

#include "protocol.h"
#include "../tml/relationfile.h"

namespace alpha {

using std::string;
using std::vector;
using std::map;
using std::ostream;
using std::is_same;
using std::endl;
using std::chrono::system_clock;

struct session {
	session_id id;
	agent a;
	session(const session_id &sid, agent &a) : id(sid), a(a) {};
};

namespace protocol {

tml::relationfile agent_db(  "agent",   4, "storage/agent.dtml");
tml::relationfile session_db("session", 3, "storage/session.dtml");
tml::relationfile channel_db("channel", 4, "storage/channel.dtml");
tml::relationfile message_db("message", 6, "storage/message.dtml");

void init() {
	agent_db.load();
	session_db.load();
	channel_db.load();
	message_db.load();
}

vector<agent>   agents{};
vector<session> sessions{};
vector<channel> channels{};
vector<message> messages{};

map<notification, vector<on_notify>> notifications_map{};
vector<ignoration> ignorations{};

enum object_types { AGENT, CHANNEL, MESSAGE };

template <typename T>
constexpr int object_type(){
	if constexpr(is_same<T,agent>::value)   return AGENT;
	if constexpr(is_same<T,channel>::value) return CHANNEL;
	if constexpr(is_same<T,message>::value) return MESSAGE;
	return -1;
}

template <typename T>
constexpr tml::relationfile& db() {
	if constexpr(is_same<T,agent>::value)   return agent_db;
	if constexpr(is_same<T,channel>::value) return channel_db;
	if constexpr(is_same<T,message>::value) return message_db;
	throw 0;
}

template<typename T>
bool match_filter(const filter& f, const T& o) {
	if constexpr(is_same<T,agent>::value)	return ((agent_f&)f).match(o);
	if constexpr(is_same<T,channel>::value) return ((channel_f&)f).match(o);
	if constexpr(is_same<T,message>::value) return ((message_f&)f).match(o);
	throw 0;
}

template <typename T>
ostream& info(T v) { return std::cout << "nonet/" << v << ": "; }

bool register_agent(agent &a) {
	//info("register_agent")<<" "<<a.name<<endl;
	if (agent_db.exists(a.name, 1)) {
		info("register_agent")
			<< "agent with such name already exists.";
		return false;
	}

	std::stringstream ss;
	ss << agent_db.max()+1;
	agent_id aid(ss.str());

	a.id = aid;
	a.created = system_clock::now();
	agent_db.insert(a.id, a.name, a.other_name, print_date(a.created));
	return true;
}

session_id login(agent_id aid) {
	tml::row ar = agent_db.find(aid);
	if (!ar.size()) {
		info("login")<<"unknown agent_id "<<aid<<endl;
		return {};
	}
	agent a(ar);
	tml::row sr = session_db.find(aid, 1);
	if (sr.size()) {
		info("login")<<"agent "<<aid
		<<" already logged in with session_id: "<<sr[0]<<endl;
		return sr[0];
	}

	std::stringstream ss; ss << session_db.max()+1;
	session_id sid{ss.str()};

	session_db.insert(sid, aid, now());

	return sid;
}

bool logout(const session_id &sid) {
	tml::row sr = session_db.find(sid);
	if (!sr.size()) {
		info("logout")<<"unknown session_id "<<sid<<endl;
		return false;
	}
	session_db.erase(sid);
	return true;
}

bool loggedin(const session_id &sid) {
	return session_db.find(sid).size();
}

bool logged_agent(const session_id &sid, agent &a) {
	tml::row sr = session_db.find(sid);
	if (sr.size() <= 1) return false;
	tml::row ar = agent_db.find(sr[1]);
	a = agent(ar);
	return true;
}

bool unregister_agent(const session_id& sid, agent& a) {
	if (!loggedin(sid)) return false;
	// info("unregister_agent")<<"("<<a.id<<") "<<a.name<<endl;
	agent_db.erase(a.id);
	return true;
}

bool create_channel(const session_id& sid, channel& ch) {
	if (!loggedin(sid)) {
		info("create_channel")<<" not logged in. cannot create channel.";
		return false;
	}

	std::stringstream ss; ss << channel_db.max()+1;
	ch.id = ss.str();

	agent a;
	if (!logged_agent(sid, a)) {
		info("create_channel")<<" cannot find logged agent.";
		return false;
	}
	ch.creator = a.id;
	ch.created = system_clock::now();
	channel_db.insert(ch.id, ch.creator, ch.name, print_date(ch.created));
	return true;
}

void check_notifications(const message& m) {
	auto it = notifications_map.begin();
	for (; it != notifications_map.end(); it++)
		if (it->first.match(m))
			for (auto fn : it->second) fn(message_ids{m.id});
}

bool send(const session_id &sid, message& m) {
	agent a;
	if (!logged_agent(sid, a)) {
		info("send") << "not logged in. cannot send";
		return false;
	}

	std::stringstream ss; ss << message_db.max()+1;
	m.id = ss.str();
	//info("send")<<"("<<m.id<<") "<<m.subject<<": "<<m.content<<endl;
	m.author = a.id;
	m.created = system_clock::now();

	ss.str("");
	for (auto it = m.targets.begin(); it != m.targets.end(); ++it)
		ss << (it != m.targets.begin() ? "," : "") << *it;
	string targets = ss.str();

	message_db.insert(m.id, m.author, targets, m.subject, m.content, now());
	check_notifications(m);
	return true;
}

bool unsend(const session_id &sid, message& m) {
	if (!loggedin(sid)) return false;
	message_db.erase(m.id);
	check_notifications(m);
	return true;
}

bool update(const session_id &sid, message& m) {
	if (!loggedin(sid)) return false;

	tml::row mr = message_db.find(m.id);
	if (!mr.size()) return false; // no such message exists

	message_db.erase(m.id);

	std::stringstream ss;
	for (auto it = m.targets.begin(); it != m.targets.end(); ++it)
		ss << (it != m.targets.begin() ? "," : "") << *it;
	string targets = ss.str();

	message_db.insert(mr[0], mr[1], targets, m.subject, m.content, mr[5]);
	check_notifications(m);
	return true;
}

bool notify(const session_id& sid, notification& n, on_notify fn) {
	if (!loggedin(sid)) return false;
	auto it = notifications_map.find(n);
	if (it != notifications_map.end()) it->second.push_back(fn);
	else notifications_map.emplace(n, vector{ fn });
	return true;
}

bool unnotify(const session_id& sid, notification& n) {
	if (!loggedin(sid)) return false;
	auto it = notifications_map.find(n);
	if (it != notifications_map.end()) notifications_map.erase(it);
	return true;
}

std::vector<notification> list_notifications(const session_id& sid) {
	std::vector<notification> r{};
	if (!loggedin(sid)) return r;
	for (auto const& it : notifications_map) r.push_back(it.first);
	return r;
}

template<typename T> // query for ids by filter
unique_ids query(const session_id& sid, const filter& f) {
	if (!loggedin(sid)) return {};
	unique_ids r;
	tml::table t = db<T>().get_relation().get_table();
	for (const auto& row : t) {
		T o(row);
		if (match_filter(f, o)) r.push_back(o.id);
	}
	return r;
}

template <typename T> // fetch objects by ids
vector<T> fetch(const session_id &sid, vector<unique_id> ids) {
	if (!loggedin(sid)) return {};
	vector<T> r;
	for (auto& id : ids) {
		tml::row row = db<T>().find(id);
		if (row.size()) r.push_back(T(row));
	}
	return r;
}

template<typename T> // query for objects by filter
vector<T> query_fetch(const session_id& sid, const filter& f) {
	if (!loggedin(sid)) return {};
	vector<T> r;
	const tml::table& t = db<T>().get_relation().get_table();
	for (auto row : t) {
		T o(row);
		if (match_filter(f, o)) r.push_back(o);
	}
	return r;
}

template agent_ids             query<agent>(  const session_id&, const filter&);
template channel_ids           query<channel>(const session_id&, const filter&);
template message_ids           query<message>(const session_id&, const filter&);
template vector<agent>         fetch<agent>(  const session_id&, agent_ids);
template vector<channel>       fetch<channel>(const session_id&, channel_ids);
template vector<message>       fetch<message>(const session_id&, message_ids);
template vector<agent>   query_fetch<agent>(  const session_id&, const filter&);
template vector<channel> query_fetch<channel>(const session_id&, const filter&);
template vector<message> query_fetch<message>(const session_id&, const filter&);

}

}
