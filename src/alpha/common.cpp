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
#include <iomanip>
#include <sstream>
#include "defs.h"

namespace alpha {

using std::string;
using std::istream;
using std::stringstream;

istream& operator>>(istream& is, timestamp& ts) {
	std::string d;
	is >> d;
	ts = parse_date(d);
	return is;
}

timestamp parse_date(const std::string& date) {
	int y, mo, d, h, m;
	float s;
	tm time;

	sscanf(date.c_str(), "%d-%d-%dT%d:%d:%f", &y, &mo, &d, &h, &m, &s);
	time.tm_year = y - 1900;
	time.tm_mon = mo - 1;
	time.tm_mday = d;
	time.tm_hour = h;
	time.tm_min = m;
	time.tm_sec = (int)s;  // FIX. looses information

	return std::chrono::system_clock::from_time_t(std::mktime(&time));
}

string print_date(const timestamp& ts) {
	stringstream ss;
	ss << ts;
	return ss.str();
}

string now() {
	return print_date(std::chrono::system_clock::now());
}

strings split(string str, char delimiter) {
    strings r{};
    stringstream ss(str);
    string s;
    while (std::getline(ss, s, delimiter)) r.push_back(s);
    return r;
}

}
