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
#include "filter.h"

namespace alpha {

using std::ostream;

inline std::string con(bool first) { return first ? " " : ", "; }

ostream& operator<<(ostream& os, const message& m) {
	os
		<< "message{ id: `"<<m.id
		<< "`, author: `"<<m.author
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
		<< "` }";
}

ostream& operator<<(ostream& os, const agent& a) {
	return os
		<< "agent{ id: `"<<a.id
		<< "`, name: `"<<a.name
		<< "`, other_name: `"<<a.other_name
		<< "`, password: ******************** }";
}

ostream& operator<<(ostream& os, const channel& ch) {
	return os
		<< "channel{ id: `"<<ch.id
		<< "`, name: `"<<ch.name
		<< "`, creator: `"<<ch.creator
		<< "` }";
}

ostream& operator<<(ostream& os, const strings& strs) {
	bool first{true};
	os << "strings{";
	for (auto &s : strs)
		os << con(first) << '"' << s << '"', first = false;
	return os << " }";
}

ostream& operator<<(ostream& os, const message_f& f) {
	bool first{true};
	os << "message_filter{";
	if (f.id != "") os << "id: `" << f.id << "`", first = false;
	if (f.author != "")
		os << con(first) << "author: `" << f.author << "`",
		first = false;
	if (f.subject != "")
		os << con(first) << "subject: `" << f.subject << "`",
		first = false;
	if (f.targets.size()) {
		bool firstt = true;
		os << con(first) << "targets: [", first = false;
		for (auto t : f.targets)
			os << con(firstt) << t, firstt = false;
		os << " ]";
	}
	if (f.content != "")
		os << con(first) << "content: `" << f.content << "`";
	return os << " }";
}

ostream& operator<<(ostream& os, const channel_f& f) {
	bool first{true};
	os << "channel_filter{";
	if (f.id != "") os << "id: `" << f.id << "`", first = false;
	if (f.creator != "")
		os << con(first) << "creator: `" << f.creator << "`",
		first=false;
	if (f.name != "") os << con(first) << "name: `" << f.name << "`";
	return os << " }";
}

ostream& operator<<(ostream& os, const agent_f& f) {
	bool first{true};
	os << "agent_filter{ ";
	if (f.id != "") os << "id: `" << f.id << "`, ", first = false;
	if (f.name != "")
		os << con(first) << "name: `" << f.name << "`, ",
		first=false;
	if (f.other_name != "")
		os << con(first) << "other_name: `" << f.other_name << "`, ";
	return os << " }";
}

}
