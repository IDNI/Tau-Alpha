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
#ifndef __ALPHA_FILTER_H__
#define __ALPHA_FILTER_H__
#include <regex>

#include "defs.h"
#include "agent.h"
#include "channel.h"
#include "message.h"

namespace alpha {

// TODO match functions for GROUPS

struct filter { };

struct terminal : public filter {};

struct group : public filter {
	typedef std::vector<filter> filters;
	filters f;
	group(filters f) : f(f) {}
};

struct and_ : public group {
	and_(filters f) : group(f) {}
};

struct or_ : public group {
	or_(filters f) : group(f) {}
};

struct not_ : public group {
	not_(filter f1) : group(filters{f1}) {}
};

struct message_f : public terminal {
	message_id id; // 0 = null
	agent_id author; // 0/"" = null
	std::string subject; // "" = null
	//timerange created; // { 0, 0 } is null
	//timerange publish; // should not match for messages published in
	//timerange valid;   // ...the future if not authored by the agent
	channel_ids targets;   // {} = null
	std::string content;   // {} = null
	//std::vector<message_id> reacts_to; // {} = null
	bool match(const message& m) const;
};

typedef message_f notification;

typedef message_f ignoration;

struct channel_f : public terminal {
	channel_id id;
	agent_id creator;
	std::string name;
	//timerange created;
	bool match(const channel& ch) const;
};

struct agent_f : public terminal {
	agent_id id;
	std::string name;
	std::string other_name;
	//std::string email;
	bool match(const agent& a) const;
};

bool operator<(const message_f&, const message_f&);
bool operator<(const channel_f&, const channel_f&);
bool operator<(const agent_f&, const agent_f&);

std::ostream& operator<<(std::ostream& os, const message_f& mf);
std::ostream& operator<<(std::ostream& os, const channel_f& chf);
std::ostream& operator<<(std::ostream& os, const agent_f& af);

}

#endif
