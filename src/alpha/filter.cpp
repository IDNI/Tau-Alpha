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
#include "filter.h"

namespace alpha {

bool message_f::match(const message& m) const {
	if (id != "" && id != m.id) return false;
	if (author != "" && author != m.author) return false;
	if (subject != "" && std::regex_match(m.subject, std::regex(subject)))
		return false;
	if (targets.size()) {
		for (auto chid : m.targets)
			for (auto t : targets)
				if (chid == t) return true;
		return false;
	}
	if (content != "" && std::regex_match(m.content, std::regex(content)))
		return false;
	return true;
}

bool channel_f::match(const channel& ch) const {
	if (id != "" && id != ch.id) return false;
	if (op != "" && op != ch.op) return false;
	if (name != "" && !std::regex_match(ch.name,std::regex(name)))
		return false;
	return true;
}

bool agent_f::match(const agent& a) const {
	if (id != "" && id != a.id) return false;
	if (name != "" && !std::regex_match(a.name, std::regex(name)))
		return false;
	if (other_name != "" && !std::regex_match(a.other_name,
							std::regex(other_name)))
		return false;
	return true;
}

}
