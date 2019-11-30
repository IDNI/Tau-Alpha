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
#include "relationfile.h"
#include "query.h"

namespace tml {

using std::string;
using std::wstring;

relationfile::relationfile(const std::string& rn, int arity,
	const string& dfile, const string& pfile, strings args)
	: datafile(dfile, pfile, args), rn(rn), arity(arity) {}
relationfile::relationfile(const std::string& rn, int arity,
	const std::string& dfile, strings args)
	: relationfile(rn, arity, dfile, "", args) {}

int relationfile::max(int col, int min) {
	return datafile::max(rn, col, min);
}

row relationfile::find(std::string search, int col) {
	return datafile::find(rn, search, col);
}

bool relationfile::exists(std::string search, int col) {
	return datafile::exists(rn, arity, search, col);
}

void relationfile::erase(std::string search, int col) {
	datafile::erase(rn, arity, search, col);
}

void relationfile::insert__(size_t n, ...) {
	va_list args;
	va_start(args, n);
	update(query::insert_args__(rn, n, args));
	va_end(args);
}

}
