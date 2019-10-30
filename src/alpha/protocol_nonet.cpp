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

using std::is_same;

struct session {
	session_id id;
	agent a;
	session(session_id sid, agent a) : id(sid), a(a) {};
};

std::vector<agent> agents{};
std::vector<session> sessions{};
std::vector<channel> channels{};
std::vector<message> messages{};

std::map<agent_id,   agent*> agents_map{};
std::map<session_id, session*> sessions_map{};
std::map<channel_id, channel*> channels_map{};
std::map<message_id, message*> messages_map{};

std::map<channel_id, std::vector<message_id>> channel_messages_map{};

namespace protocol {

enum object_types { AGENT, CHANNEL, MESSAGE };

template <typename T>
constexpr int object_type(){
	if constexpr(is_same<T,agent>::value)   return AGENT;
	if constexpr(is_same<T,channel>::value) return CHANNEL;
	if constexpr(is_same<T,message>::value) return MESSAGE;
	return -1;
}

template <typename T>
constexpr std::vector<T>& objects() {
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
std::ostream& info(T v) { return std::cout << "nonet/" << v << ": "; }

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
	info("register_agent")<<"("<<aid<<") "<<a.name<<std::endl;
	a.id = aid;
	agents.push_back(a);
	agents_map.emplace(aid, &(agents.back()));
	return true;
}

session_id login(agent_id aid, std::string password) {
	auto it = agents_map.find(aid);
	if (it == agents_map.end()) {
		info("login")<<"unknown agent_id "<<aid<<std::endl;
		return {};
	}
	agent* a = it->second;

	for (auto& s : sessions) if (s.a.id == aid) {
		info("login")<<"agent "<<aid
		<<" already logged in with session_id: "<<s.id<<std::endl;
		return s.id;
	}

	if (password != a->password) {
		info("login")<<"wrong password"<<std::endl;
		return {};
	}

	static size_t _id{0};
	std::stringstream ss; ss << ++_id;
	session_id sid{ss.str()};
	sessions.emplace_back(sid, *a);
	sessions_map.emplace(sid, &(sessions.back()));
	info("login")<<"("<<_id<<") "<<a->name<<" session "<<sid<<std::endl;
	return sid;
}

bool logout(const session_id &sid) {
	auto it = sessions_map.find(sid);
	if (it == sessions_map.end()) {
		info("logout")<<"unknown session_id "<<sid<<std::endl;
		return false;
	}
	info("logout")<<it->second->a.name
		<<" ("<<it->second->a.id<<")"<<std::endl;
	for (auto sit = sessions.begin(); sit != sessions.end(); ++sit)
		if (sit->id == it->second->id) {
			sessions_map.erase(it);
			sessions.erase(sit);
			return true;
		}
	info("logout")<<"session in map but not in list "<<sid<<std::endl;
	throw 0;
	return false;
}

bool loggedin(const session_id &sid) {
	info("loggedin") << "sid: " << sid << " t/f:"
	<< (sessions_map.find(sid) != sessions_map.end()) << std::endl;
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

bool create_channel(const session_id& sid, channel& ch) {
	if (!loggedin(sid)) {
		info("send") << "not logged in. cannot send";
		return false;
	}

	static size_t _id{0};
	std::stringstream ss; ss << ++_id;
	info("create_channel")<<"("<<_id<<") "<<ch.name<<std::endl;
	ch.id = ss.str();
	agent a;
	logged_agent(sid, a);
	ch.op = a.id;
	channels.push_back(ch);
	channels_map.emplace(ch.id, &(channels.back()));
	return true;
}

bool send(const session_id &sid, message& m) {
	agent a;
	if (!logged_agent(sid, a)) {
		info("send") << "not logged in. cannot send";
		return false;
	}
	static size_t _id{0};
	std::stringstream ss; ss << ++_id;
	info("send")<<"("<<_id<<") "<<m.subject<<": "<<m.content<<std::endl;
	m.id = ss.str();
	m.author = a.id;
	messages.push_back(m);
	messages_map.emplace(m.id, &(messages.back()));
	return true;
}

template<typename T> // query for ids
unique_ids query(const session_id& sid, const filter& f) {
	if (!loggedin(sid)) return {};
	unique_ids r;
	for (T& o : objects<T>())
		if (match_filter(f, o))	r.push_back(o.id);
	return r;
}

template <typename T> // fetch object by ids
std::vector<T> fetch(const session_id &sid, std::vector<unique_id> ids) {
	if (!loggedin(sid)) return {};
	std::vector<T> r;
	for (auto& id : ids)
		for (T e : objects<T>())
			if (e.id == id) r.push_back(e);
	return r;
}

template agent_ids query<agent>(const session_id&, const filter&);
template channel_ids query<channel>(const session_id&, const filter&);
template message_ids query<message>(const session_id&, const filter&);
template std::vector<agent> fetch<agent>(const session_id&, agent_ids ids);
template std::vector<channel> fetch<channel>(const session_id&, channel_ids ids);
template std::vector<message> fetch<message>(const session_id&, message_ids ids);

}

}
