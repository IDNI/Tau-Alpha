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
// no persistence

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

#include "protocol.h"

namespace alpha {

using std::string;
using std::vector;
using std::map;
using std::ostream;
using std::is_same;
using std::endl;

struct session {
	session_id id;
	agent a;
	session(const session_id &sid, agent &a) : id(sid), a(a) {};
};

namespace protocol {

vector<agent>   agents{};
vector<session> sessions{};
vector<channel> channels{};
vector<message> messages{};

map<agent_id,   agent*>   agents_map{};
map<session_id, session*> sessions_map{};
map<channel_id, channel*> channels_map{};
map<message_id, message*> messages_map{};

map<notification, vector<on_notify>> notifications_map{};
vector<ignoration> ignorations{};

map<channel_id, vector<message_id>> channel_messages_map{};

enum object_types { AGENT, CHANNEL, MESSAGE };

template <typename T>
constexpr int object_type(){
	if constexpr(is_same<T,agent>::value)   return AGENT;
	if constexpr(is_same<T,channel>::value) return CHANNEL;
	if constexpr(is_same<T,message>::value) return MESSAGE;
	return -1;
}

template <typename T>
constexpr vector<T>& objects() {
	if constexpr(is_same<T,agent>::value)   return agents;
	if constexpr(is_same<T,channel>::value) return channels;
	if constexpr(is_same<T,message>::value) return messages;
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
	for (auto& sa : agents)
		if (sa.name == a.name) {
			info("register_agent")
				<< "agent with such name already exists.";
			return false;
		}
	static size_t _id{0};
	std::stringstream ss; ss << ++_id;
	agent_id aid = ss.str();
	// info("register_agent")<<"("<<aid<<") "<<a.name<<endl;
	a.id = aid;
	agents.push_back(a);
	agents_map.emplace(aid, &(agents.back()));
	return true;
}

session_id login(agent_id aid, string password) {
	auto it = agents_map.find(aid);
	if (it == agents_map.end()) {
		info("login")<<"unknown agent_id "<<aid<<endl;
		return {};
	}
	agent* a = it->second;

	for (auto& s : sessions) if (s.a.id == aid) {
		info("login")<<"agent "<<aid
		<<" already logged in with session_id: "<<s.id<<endl;
		return s.id;
	}

	if (password != a->password) {
		info("login")<<"wrong password"<<endl;
		return {};
	}

	static size_t _id{0};
	std::stringstream ss; ss << ++_id;
	session_id sid{ss.str()};
	sessions.emplace_back(sid, *a);
	sessions_map.emplace(sid, &(sessions.back()));
	// info("login")<<"("<<_id<<") "<<a->name<<" session "<<sid<<endl;
	return sid;
}

bool logout(const session_id &sid) {
	auto it = sessions_map.find(sid);
	if (it == sessions_map.end()) {
		info("logout")<<"unknown session_id "<<sid<<endl;
		return false;
	}
	// info("logout")<<it->second->a.name
	// 	<<" ("<<it->second->a.id<<")"<<endl;
	for (auto sit = sessions.begin(); sit != sessions.end(); ++sit)
		if (sit->id == it->second->id) {
			sessions_map.erase(it);
			sessions.erase(sit);
			return true;
		}
	info("logout")<<"session in map but not in list "<<sid<<endl;
	throw 0;
	return false;
}

bool loggedin(const session_id &sid) {
	// info("loggedin") << "sid: " << sid << " t/f:"
	// << (sessions_map.find(sid) != sessions_map.end()) << endl;
	return sessions_map.find(sid) != sessions_map.end();
}

bool logged_agent(const session_id &sid, agent &a) {
	auto it = sessions_map.find(sid);
	if (it == sessions_map.end()) {
		return false;
	}
	a = it->second->a;
	return true;
}

bool unregister_agent(const session_id& sid, agent& a) {
	if (!loggedin(sid)) return false;
	info("unregister_agent")<<"("<<a.id<<") "<<a.name<<endl;
	auto it = agents_map.find(a.id);
	auto ait = agents.begin();
	for (; ait != agents.end(); ++ait) if (ait->id == a.id) break;
	if (ait != agents.end()) agents.erase(ait);
	if (it != agents_map.end()) agents_map.erase(it);
	return true;
}

bool create_channel(const session_id& sid, channel& ch) {
	if (!loggedin(sid)) {
		info("send") << "not logged in. cannot send";
		return false;
	}

	static size_t _id{0};
	std::stringstream ss; ss << ++_id;
	// info("create_channel")<<"("<<_id<<") "<<ch.name<<endl;
	ch.id = ss.str();
	agent a;
	logged_agent(sid, a);
	ch.creator = a.id;
	channels.push_back(ch);
	channels_map.emplace(ch.id, &(channels.back()));
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
	static size_t _id{0};
	std::stringstream ss; ss << ++_id;
	//info("send")<<"("<<_id<<") "<<m.subject<<": "<<m.content<<endl;
	m.id = ss.str();
	m.author = a.id;
	messages.push_back(m);
	messages_map.emplace(m.id, &(messages.back()));
	check_notifications(m);
	return true;
}

bool unsend(const session_id &sid, message& m) {
	if (!loggedin(sid)) return false;
	auto it = messages_map.find(m.id);
	if (it != messages_map.end()) {
		auto mit = messages.begin();
		for (; mit != messages.end(); ++mit)
			if (mit->id == m.id) {
				messages.erase(mit);
				break;
			}
		messages_map.erase(it);
		check_notifications(m);
	}
	return true;
}

bool update(const session_id &sid, message& m) {
	if (!loggedin(sid)) return false;
	auto it = messages_map.find(m.id);
	if (it != messages_map.end()) {
		auto mit = messages.begin();
		for (; mit != messages.end(); ++mit)
			if (mit->id == m.id) {
				mit->targets = m.targets;
				mit->subject = m.subject;
				mit->content = m.content;
				check_notifications(m);
				break;
			}
	}
	return true;
}

bool notify (const session_id& sid, notification& n, on_notify fn) {
	if (!loggedin(sid)) return false;
	auto it = notifications_map.find(n);
	if (it != notifications_map.end()) it->second.push_back(fn);
	else notifications_map.emplace(n, vector{ fn });
	return true;
}

bool unnotify (const session_id& sid, notification& n) {
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
	for (T& o : objects<T>())
		if (match_filter(f, o))	r.push_back(o.id);
	return r;
}

template <typename T> // fetch objects by ids
vector<T> fetch(const session_id &sid, vector<unique_id> ids) {
	if (!loggedin(sid)) return {};
	vector<T> r;
	for (auto& id : ids)
		for (T o : objects<T>())
			if (o.id == id) r.push_back(o);
	return r;
}

template<typename T> // query for objects by filter
vector<T> query_fetch(const session_id& sid, const filter& f) {
	if (!loggedin(sid)) return {};
	vector<T> r;
	for (T& o : objects<T>())
		if (match_filter(f, o))	r.push_back(o);
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
