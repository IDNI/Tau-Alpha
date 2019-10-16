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
#include <sstream>

#include "repl.h"

namespace alpha {

using std::endl;

int repl::run() {
	bool r;
	while (std::cin && loop) r = eval(parse(read()));
	return !r;
}

std::string repl::read() {
	std::stringstream ss;
	std::string ln;
	p(prompt());
	std::getline(std::cin, ln);
	if (!std::cin) return "";
	while (ln.back() == '\\') {
		ss << ln;
		std::getline(std::cin, ln);
	}
	ss << ln;
	return ss.str();
}

strings repl::parse(std::string input) {
	std::istringstream in(input);
	std::string w{};
	strings r{};
	while (in >> w) if (in) r.push_back(w);
	return r;
}

bool repl::eval(strings command) {
	if (!command.size()) return false;
	std::string cmd{command[0]};
	if (cmd == "reg" || cmd == "register" || cmd == "register_agent")
		return register_agent(command);
	else if (cmd == "l" || cmd == "li" || cmd == "login")
		return login(command);
	else if (cmd == "lo" || cmd == "logout")
		return logout();
	else if (cmd == "s" || cmd == "send" || cmd == "m" || cmd == "msg")
		return send(command);
	else if (cmd == "cch" || cmd == "create_channel")
		return create_channel(command);
	else if (cmd == "qm" || cmd == "query_messages")
		return query_and_fetch_messages(command);
	else if (cmd == "qc" || cmd == "qch" || cmd == "query_channels")
		return query_and_fetch_channels(command);
	else if (cmd == "qa" || cmd == "query_agents")
		return query_and_fetch_agents(command);
	else if (cmd == "q" || cmd == "quit" || cmd == "x" || cmd == "exit")
		return loop = false, true;
	return false;
}

bool repl::check_args(const strings args, const size_t n_required) const {
	if (args.size() < n_required) {
		p("not enough arguments") << endl;
		return false;
	}
	return true;
}

bool repl::register_agent(strings args) {
	if (!check_args(args, 3)) return false;
	agent a(args[1], args[2], args.size() > 3 ? args[3] : "");
	p("register_agent ") << a.name << ' ' << a.other_name << endl;
	bool r;
	if (!(r = protocol::register_agent(a))) p("register_agent ERROR: ")
								<< sid << endl;
	else p("register_agent OK. agent_id: ") << a.id << endl;
	return r;
}

bool repl::login(strings args) {
	if (!check_args(args, 3)) return false;
	session_id id = protocol::login((agent_id)args[1], args[2]);
	if (id == session_id{}) { p("access denied") << endl; return false; }
	sid = id;
	p("login OK. session_id: ") << id << endl;
	return true;
}

bool repl::logout() {
	if (!protocol::logout(sid)) {
		p("logout ERROR. session_id: ") << sid << endl;
		return false;
	}
	p("logout OK. removing session_id: ") << sid << endl;
	sid = {};
	p("logout OK. removing session_id: ") << sid << endl;
	return true;
}

bool repl::send(strings args) {
	if (!check_args(args, 3)) return false;
	p("send ") << args[1] << ' ' << args[2] << endl;
	message m{}; m.targets = { args[1] }; m.content = { args[2] };
	if (!protocol::send(sid, m)) {
		p("sent ERROR") << endl;
		return false;
	}
	p("sent OK. message_id: ") << m.id << endl;
	return true;
}

bool repl::create_channel(strings args) {
	if (!check_args(args, 2)) return false;
	p("create_channel ") << args[1] << endl;
	channel ch{}; ch.name = args[1];
	if (!protocol::create_channel(sid, ch)) return false;
	p("create_channel OK. channel_id: ") << ch.id << endl;
	return true;
}

size_t repl::count_messages(strings args) {
	return query_message_ids(args).size();
}

size_t repl::count_agents(strings args) {
	return query_agent_ids(args).size();
}

size_t repl::count_channels(strings args) {
	return query_channel_ids(args).size();
}

message_ids repl::query_message_ids(strings args) {
	p("query_message_ids ") << args<< endl;
	filter::message f;
	auto it = args.begin();
	while (++it != args.end()) {
		auto filter = *it;
		if (++it == args.end()) {
			p("query_message_ids missing argument");
			return {};
		}
		auto search = *it;
		if (filter == "id") f.id = message_id(search);
		else if (filter == "author") f.author = agent_id(search);
		else if (filter == "subject") f.subject = search;
		else if (filter == "target") f.targets
						.push_back(channel_id(search));
		else if (filter == "content") f.content = search;
	}
	message_ids mids = protocol::query_messages(sid, f);
	if (!mids.size()) {
		p("no messages found") << endl;
		return {};
	}
	p("found messages (ids): ");
	for (message_id& mid : mids) p(mid) << " ";
	p("") << endl;
	return mids;
}

agent_ids repl::query_agent_ids(strings args) {
	p("query_agents ") << args << endl;
	filter::agent f;
	auto it = args.begin();
	while (++it != args.end()) {
		auto filter = *it;
		if (++it == args.end()) {
			p("query_agent_ids missing argument");
			return {};
		}
		auto search = *it;
		if (filter == "id") f.id = agent_id(search);
		else if (filter == "name") f.name = search;
		else if (filter == "other_name") f.other_name = search;
	}
	agent_ids aids = protocol::query_agents(sid, f);
	if (!aids.size()) {
		p("no agents found") << endl;
		return {};
	}
	p("found agents (ids): ");
	for (agent_id& aid : aids) p(aid) << " ";
	p("") << endl;
	return aids;
}

channel_ids repl::query_channel_ids(strings args) {
	p("query_channels ") << args << endl;
	filter::channel f;
	auto it = args.begin();
	while (++it != args.end()) {
		auto filter = *it;
		if (++it == args.end()) {
			p("query_channel_ids missing argument");
			return {};
		}
		auto search = *it;
		if (filter == "id") f.id = channel_id(search);
		else if (filter == "op") f.op = search;
		else if (filter == "name") f.name = search;
	}
	channel_ids chids = protocol::query_channels(sid, f);
	if (!chids.size()) {
		p("no channels found") << endl;
		return {};
	}
	p("found channels (ids): ");
	for (channel_id& chid : chids) p(chid) << " ";
	p("") << endl;
	return chids;
}

bool repl::query_and_fetch_messages(strings args) {
	std::vector<message> ms = protocol::fetch_messages(sid,
							query_message_ids(args));
	for (message& m : ms) p(m) << endl;
	return ms.size();
}

bool repl::query_and_fetch_agents(strings args) {
	std::vector<agent> as = protocol::fetch_agents(sid,
							query_agent_ids(args));
	for (agent& a : as) p(a) << endl;
	return as.size();
}

bool repl::query_and_fetch_channels(strings args) {
	std::vector<channel> chs = protocol::fetch_channels(sid,
							query_channel_ids(args));
	for (channel& ch : chs) p(ch) << endl;
	return chs.size();
}

}
