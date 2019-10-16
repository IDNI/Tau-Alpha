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
	std::string subject; // "" = null
	//timerange created; // { 0, 0 } is null
	//timerange publish; // should not match for messages published in
	//timerange valid;   // ...the future if not authored by the agent
	channel_ids targets;   // {} = null
	std::string content;   // {} = null
	//std::vector<message_id> reacts_to; // {} = null
	bool match(const ::alpha::message& m) const {
		if (id != "" && id != m.id) return false;
		if (author != "" && author != m.author) return false;
		if (subject != "" && std::regex_match(m.subject,
							std::regex(subject)))
			return false;
		if (targets.size()) {
			for (auto chid : m.targets)
				for (auto t : targets)
					if (chid == t) return true;
			return false;
		}
		if (content != "" && std::regex_match(m.content,
							std::regex(content)))
			return false;
		return true;
	}
};

typedef message notification;

typedef message ignoration;

struct channel : public terminal {
	channel_id id;
	agent_id op;
	std::string name;
	//timerange created;
	bool match(const ::alpha::channel& ch) const {
		if (id != "" && id != ch.id) return false;
		if (op != "" && op != ch.op) return false;
		if (name != "" && std::regex_match(ch.name,std::regex(name)))
			return false;
		return true;
	}
};

struct agent : public terminal {
	agent_id id;
	std::string name;
	std::string other_name;
	//std::string email;
	bool match(const ::alpha::agent& a) const {
		if (id != "" && id != a.id) return false;
		if (name != "" && std::regex_match(a.name, std::regex(name)))
			return false;
		if (other_name != "" && std::regex_match(a.other_name,
							std::regex(other_name)))
			return false;
		return true;
	}
};

}

}

#endif
