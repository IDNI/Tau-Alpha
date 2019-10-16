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
#include "message.h"
#include "agent.h"
#include "channel.h"

namespace alpha {

std::ostream& operator<<(std::ostream& os, const message& m) {
	os
		<< "message("<<m.id<<")"
		<< " [ author: `"<<m.author
		<< "`, targets: `";
	bool first{true};
	for (auto& t : m.targets) {
		if (!first) os << ", ";
		else first = false;
		os << t;
	}
	return os
		<< "`, subject: `"<<m.subject
		<< "`, content: `"<<m.content
		<< "` ]";
}

std::ostream& operator<<(std::ostream& os, const agent& a) {
	return os
		<< "agent("<<a.id<<")"
		<< " [ name: `"<<a.name
		<< "`, other_name: `"<<a.other_name
		<< "`, password: ******************** ]";
}

std::ostream& operator<<(std::ostream& os, const channel& ch) {
	return os
		<< "channel("<<ch.id<<")"
		<< " [ name: `"<<ch.name
		<< "`, op: `"<<ch.op
		<< "` ]";
}

std::ostream& operator<<(std::ostream& os, const strings& strs) {
	bool first{true};
	os << "[ ";
	for (std::string s : strs) {
		if (!first) os << ", ";
		else first = false;
		os << '"' << s << '"';
	}
	return os << " ]";
}

}
