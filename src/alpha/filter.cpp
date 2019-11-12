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

regex re(std::string s) {
	std::stringstream ss;
	ss << ".*" << s << ".*";
	return regex(ss.str());
}

bool message_f::match(const message& m) const {
	if (id != "" && id != m.id) return false;
	if (author != "" && author != m.author) return false;
	if (subject != "" && !regex_match(m.subject, re(subject)))
		return false;
	if (targets.size()) {
		for (auto chid : m.targets)
			for (auto t : targets)
				if (chid == t) return true;
		return false;
	}
	if (content != "" && !regex_match(m.content, re(content)))
		return false;
	return true;
}

bool channel_f::match(const channel& ch) const {
	if (id != "" && id != ch.id) return false;
	if (creator != "" && creator != ch.creator) return false;
	if (name != "" && !regex_match(ch.name, re(name)))
		return false;
	return true;
}

bool agent_f::match(const agent& a) const {
	if (id != "" && id != a.id) return false;
	if (name != "" && !regex_match(a.name, re(name)))
		return false;
	if (other_name != "" && !regex_match(a.other_name, re(other_name)))
		return false;
	return true;
}

bool operator<(const message_f& a, const message_f& b) {
	if (a.id != b.id) return a.id < b.id;
	if (a.author != b.author) return a.author < b.author;
	if (a.subject != b.subject) return a.subject < b.subject;
	if (a.targets != b.targets) return a.targets < b.targets;
	if (a.content != b.content) return a.content < b.content;
	return false;
}

bool operator<(const channel_f& a, const channel_f& b) {
	if (a.id != b.id) return a.id < b.id;
	if (a.creator != b.creator) return a.creator < b.creator;
	if (a.name != b.name) return a.name < b.name;
	return false;
}

bool operator<(const agent_f& a, const agent_f& b) {
	if (a.id != b.id) return a.id < b.id;
	if (a.name != b.name) return a.name < b.name;
	if (a.other_name != b.other_name) return a.other_name < b.other_name;
	return false;
}

}
