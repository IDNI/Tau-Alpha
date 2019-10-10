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

#include "defs.h"

namespace alpha {

namespace filter {

struct filter {};

struct terminal : public filter { terminal() {} };

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

struct message : public terminal {
	message_id id; // 0 = null
	agent_id author; // 0/"" = null
	std::string topic; // "" = null
	timerange created; // { 0, 0 } is null
	timerange publish; // should not match for messages published in
	timerange valid;   // ...the future if not authored by the agent
	std::vector<channel_id> targets;   // {} = null
	std::vector<message_id> reacts_to; // {} = null
};

typedef message notification;

typedef message ignoration;

struct channel : public terminal {
	channel_id id;
	agent_id op;
	std::string title;
	timerange created;
};

struct agent : public terminal {
	agent_id id_;
	std::string name_;
	std::string other_name_;
	std::string email_;
};

}

}

#endif
