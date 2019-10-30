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

using std::regex_match;
using std::regex;

bool message_f::match(const message& m) const {
	if (id != "" && id != m.id) return false;
	if (author != "" && author != m.author) return false;
	if (subject != "" && regex_match(m.subject, regex(subject)))
		return false;
	if (targets.size()) {
		for (auto chid : m.targets)
			for (auto t : targets)
				if (chid == t) return true;
		return false;
	}
	if (content != "" && regex_match(m.content, regex(content)))
		return false;
	return true;
}

bool channel_f::match(const channel& ch) const {
	if (id != "" && id != ch.id) return false;
	if (op != "" && op != ch.op) return false;
	if (name != "" && !regex_match(ch.name,regex(name)))
		return false;
	return true;
}

bool agent_f::match(const agent& a) const {
	if (id != "" && id != a.id) return false;
	if (name != "" && !regex_match(a.name, regex(name)))
		return false;
	if (other_name != "" && !regex_match(a.other_name,
							regex(other_name)))
		return false;
	return true;
}

}
