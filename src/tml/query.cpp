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
#include <sstream>
#include <iostream>
#include <stdarg.h>
#include "query.h"

namespace tml {

using std::string;
using std::stringstream;

string query::insert_args__(const string& name, size_t n, va_list args) {
	stringstream ss;
 	ss << name;
 	if (n) {
 		ss << "(";
 		for (size_t i = 0; i != n; ++i) {
 			ss << (i?" ":"") << stringify(va_arg(args, string));
 		}
 		ss << ")";
 	}
 	ss << ".\n";
	return ss.str();
}

string query::insert__(const string& name, size_t n, ...) {
	va_list args;
	va_start(args, n);
	string r = insert_args__(name, n, args);
	va_end(args);
	return r;
}

string query::update__(const string& name, int arity, string search, int col,
	size_t n, ...)
{
	va_list args;
	stringstream ss;
	ss << erase(name, arity, search, col);
	va_start(args, n);
	ss << insert_args__(name, n, args);
	va_end(args);
	return ss.str();
}

string query::erase(const string& name, int arity, string search, int col) {
	stringstream ss, ss2;
	ss << "~";
	ss2 << name;
	if (arity) {
		if (search == "")
			return "# no search provided for erasing in "+name+"\n";
		ss2 << "(";
		for (int i = 0, n = 0; i != arity; ++i) {
			if (i) ss2 << " ";
			if (i == col) ss2 << stringify(search);
			else ss2 << "?U" << n++;
		}
		ss2 << ")";
		ss << ss2.str();
	}
	ss << ":-" << ss2.str() << ".\n";
	return ss.str();
}

string query::exists(const string& name, int arity, string search, int col) {
	stringstream ss;
	ss << "exists(" << stringify(name) << " " << arity << " "
		<< stringify(search) << " " << col << ") :- " << name;
	if (arity) {
		if (search == "")
			return "# no search provided for existence check in " +
				name + "\n";
		ss << "(";
		for (int i = 0, n = 0; i != arity; ++i) {
			if (i) ss << " ";
			if (i == col) ss << stringify(search);
			else ss << "?U" << n++;
		}
		ss << ")";
	}
	ss << ".\n";
	return ss.str();
}

string query::toggle_enabled(const string& ent, bool enable) {
	std::stringstream ss;
	ss << (enable ? "enable" : "disable") << "(" << stringify(ent) << ").";
	return ss.str();
}

string query::toggle_selected(const string& ent, const string& o, bool slct) {
	std::stringstream ss;
	ss<<(slct?"":"un")<<"select("<<stringify(ent)<<" "<<stringify(o)<<").";
	return ss.str();

}

string query::escape(const string& unescaped) {
	stringstream ss;
	for (auto c : unescaped) {
		if (c == '\\') ss << "\\\\";
		else if (c == '"') ss << "\\\"";
		else ss << c;
	}
	return ss.str();
}

string query::stringify(const string& s) {
	stringstream ss;
	ss << '"' << escape(s) << '"';
	return ss.str();
}

}
