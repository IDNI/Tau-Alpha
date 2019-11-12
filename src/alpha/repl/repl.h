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
#ifndef __ALPHA_REPL_H__
#define __ALPHA_REPL_H__
#include <iostream>
#include <string>
#include <vector>

#include "../protocol.h"

namespace alpha {

typedef std::vector<std::string> strings;

class repl {
	int argc;
	char** argv;
	bool loop{true};
	session_id sid;
	agent a;

	std::string prompt() const { return "> "; };
	strings parse(std::string input);
	std::string read();
	bool eval(strings command);

	bool check_args(const strings args, const size_t n_required) const;
	template<typename T>
	std::ostream& print(const T v) const { return std::cout << v; }
	template<typename T>
	std::ostream& p(const T v) const { return print(v); }

	bool register_agent(strings args);
	bool login(strings args);
	bool logout();
	bool send(strings args);
	bool unsend(strings args);
	bool update(strings args);

	bool notify(strings args);
	bool unnotify(strings args);
	bool list_notifications();

	bool create_channel(strings args);

	size_t count_messages(strings args);
	size_t count_agents(strings args);
	size_t count_channels(strings args);

	message_ids query_message_ids(strings args);
	agent_ids query_agent_ids(strings args);
	channel_ids query_channel_ids(strings args);

	bool query_and_fetch_messages(strings args);
	bool query_and_fetch_agents(strings args);
	bool query_and_fetch_channels(strings args);


public:
	repl(int argc, char** argv) : argc(argc), argv(argv) {};
	int run();
};

}

#endif
