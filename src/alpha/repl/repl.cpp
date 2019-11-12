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

using std::string;
using std::vector;
using std::endl;

string concat_rest(strings args, const size_t from) {
	std::stringstream ss;
	for (size_t i = from; i < args.size(); ++i) {
		if (i != from) ss << " ";
		ss << args[i];
	}
	return ss.str();
}

int repl::run() {
	bool r{true};
	while (std::cin && loop) {
		r = eval(parse(read()));
		if (r) p("") << endl;
	}
	return !r;
}

string repl::read() {
	std::stringstream ss;
	string ln;
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

strings repl::parse(string input) {
	std::istringstream in(input);
	string w{};
	strings r{};
	while (in >> w) if (in) r.push_back(w);
	if (!r.size()) p("") << endl;
	return r;
}

bool repl::eval(strings command) {
	if (!command.size()) return false;
	string cmd{command[0]};
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
	else if (cmd == "notify" || cmd == "trigger")
		return notify(command);
	else if (cmd == "unnotify" || cmd == "untrigger")
		return unnotify(command);
	else if (cmd == "unsend" || cmd == "delete" || cmd == "remove")
		return unsend(command);
	else if (cmd == "update" || cmd == "edit" || cmd == "change")
		return update(command);
	else if (cmd == "list_notifications" || cmd == "ln")
		return list_notifications();
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
	agent a(args[1], args[2], concat_rest(args, 3));
	p("register_agent ") << a.name << ' ' << a.other_name << endl;
	bool r;
	if (!(r = protocol::register_agent(a))) p("register_agent ERROR: ")
								<< sid << endl;
	else p("register_agent OK. agent_id: ") << a.id << endl;
	return r;
}

bool repl::login(strings args) {
	if (!check_args(args, 3)) return false;
	p("login ") << args[1] << endl;
	session_id id = protocol::login((agent_id)args[1], args[2]);
	if (id == session_id{}) { p("access denied") << endl; return false; }
	sid = id;
	p("login OK. session_id: ") << id << endl;
	return true;
}

bool repl::logout() {
	p("logout") << endl;
	if (!protocol::logout(sid)) {
		p("logout ERROR. session_id: ") << sid << endl;
		return false;
	}
	p("logout OK. removing session_id: ") << sid << endl;
	sid = {};
	return true;
}

bool repl::send(strings args) {
	if (!check_args(args, 3)) return false;
	p("send ") << args[1] << ' ' << concat_rest(args, 2) << endl;
	message m{}; m.targets = { args[1] }; m.content = {concat_rest(args,2)};
	if (!protocol::send(sid, m)) {
		p("send ERROR") << endl;
		return false;
	}
	p("send OK. message_id: ") << m.id << endl;
	return true;
}

bool repl::unsend(strings args) {
	if (!check_args(args, 2)) return false;
	p("unsend ") << args[1] << endl;
	message m{}; m.id = { args[1] };
	if (!protocol::unsend(sid, m)) {
		p("unsend ERROR") << endl;
		return false;
	}
	p("unsend OK. message_id: ") << m.id << endl;
	return true;

}

bool repl::update(strings args) {
	if (!check_args(args, 3)) return false;
	p("update ") << args[1] << ' ' << concat_rest(args, 2) << endl;
	auto ms = protocol::fetch<message>(sid, message_ids{ args[1] });
	if (!ms.size()) return false;
	ms[0].content = { concat_rest(args, 2) };
	if (!protocol::update(sid, ms[0])) {
		p("update ERROR") << endl;
		return false;
	}
	p("update OK. message_id: ") << ms[0].id << endl;
	return true;
}

bool repl::notify(strings args) {
	if (!check_args(args, 2)) return false;
	p("notify ") << args[1] << endl;
	notification n{}; n.targets = { args[1] };
	bool r = protocol::notify(sid, n, [](message_ids ids) {
		std::cout << "on_notify(" << ids[0] << ")" << endl;
	});
	p(r?"notify OK":"notify ERROR")<<endl;
	return r;
}

bool repl::unnotify(strings args) {
	if (!check_args(args, 2)) return false;
	p("unnotify ") << args[1] << endl;
	notification n{}; n.targets = { args[1] };
	if (protocol::unnotify(sid, n)) p("unnotify OK") << endl;
	else return p("unnotify ERROR") << endl, false;
	return true;
}

bool repl::list_notifications() {
	auto ns = protocol::list_notifications(sid);
	p("list_notifications") << endl;
	if (!ns.size()) p("no notifications found.");
	for (auto n : ns) p("notification ") << n << endl;
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
	p("query_message_ids ") << args << endl;
	message_f f;
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
	message_ids mids = protocol::query<message>(sid, f);
	if (!mids.size()) {
		p("no messages found") << endl;
		return {};
	}
	// p("found messages (ids): ");
	// for (message_id& mid : mids) p(mid) << " ";
	// p("") << endl;
	return mids;
}

agent_ids repl::query_agent_ids(strings args) {
	p("query_agents ") << args << endl;
	agent_f f;
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
	agent_ids aids = protocol::query<agent>(sid, f);
	if (!aids.size()) {
		p("no agents found") << endl;
		return {};
	}
	// p("found agents (ids): ");
	// for (agent_id& aid : aids) p(aid) << " ";
	// p("") << endl;
	return aids;
}

channel_ids repl::query_channel_ids(strings args) {
	p("query_channels ") << args << endl;
	channel_f f;
	auto it = args.begin();
	while (++it != args.end()) {
		auto filter = *it;
		if (++it == args.end()) {
			p("query_channel_ids missing argument");
			return {};
		}
		auto search = *it;
		if (filter == "id") f.id = channel_id(search);
		else if (filter == "creator") f.creator = search;
		else if (filter == "name") f.name = search;
	}
	channel_ids chids = protocol::query<channel>(sid, f);
	if (!chids.size()) {
		p("no channels found") << endl;
		return {};
	}
	// p("found channels (ids): ");
	// for (channel_id& chid : chids) p(chid) << " ";
	// ("") << endl;
	return chids;
}

bool repl::query_and_fetch_messages(strings args) {
	vector<message> ms = protocol::fetch<message>(sid,
						query_message_ids(args));
	for (message& m : ms) p(m) << endl;
	return ms.size();
}

bool repl::query_and_fetch_agents(strings args) {
	vector<agent> as = protocol::fetch<agent>(sid,
						query_agent_ids(args));
	for (agent& a : as) p(a) << endl;
	return as.size();
}

bool repl::query_and_fetch_channels(strings args) {
	vector<channel> chs = protocol::fetch<channel>(sid,
						query_channel_ids(args));
	for (channel& ch : chs) p(ch) << endl;
	return chs.size();
}

}
