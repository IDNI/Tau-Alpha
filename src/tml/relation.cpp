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
//#include <iostream>

#include "relation.h"
#include "query.h"

namespace tml {

using std::string;
using std::wstring;
using std::vector;
using std::ostream;

relation::relation(string n) : valid_(true), n(n) {}

bool relation::row_exist(int row) const {
	try {
		t.at(row);
		return true;
	} catch (const std::out_of_range&) {
		return false;
	}
}

vector<string>& relation::get_row(int row) {
	return t.at(row);
}

int relation::find_row(string search, int col) const {
	if (search == "" || col < 0 || col >= nc) {
		std::cout << "invalid args for find_row('" << search << "', " << col << ")" << std::endl;
		return -1;
	}
	// std::cout <<"find_row('"<<search<<"', "<<col<<")"<<std::endl
	// 	<<"\tsearching in "<<n<<"/"<<nc<<" ("<<nr<<" rows)"<< std::endl;
	if (!nr || !nc) return -1; // no rows, no cols
	int   from = col == -1 ?    0 : col,
		to = col == -1 ? nc-1 : col;
	// std::cout << "\tlimit search to columns: "<<from<<"-"<<to<<std::endl;
	for (int i = 0; i != nr; ++i)
		for (int j = from; j <= to; ++j) {
			// std::cout << "\tcheck([" << i << "," << j << "]) "
			// 	<< t.at(i).at(j) << " "
			// 	<< (t.at(i).at(j) == search ? "true" : "false")
			// 	<< std::endl;
			if (t.at(i).at(j) == search) return i;
		}
	return -1;
}

string relation::get(int row, int column) const {
	try {
		return t.at(row).at(column);
	} catch (const std::out_of_range&) {
		return "";
	}
}

string &relation::set(int row, int column, string value) {
	if (nr <= row) t.resize(row + 1), nr = row + 1;
	if (nc <= column) nc = column + 1;
	auto& r = t.at(row);
	if (r.size() <= reinterpret_cast<size_t&>(column)) r.resize(column + 1);
	return t.at(row).at(column) = value;
}

relation relation::find(const relations& r, const wstring& name) {
	auto it = r.find(name);
	if (it != r.end()) return *(it->second);
	return relation{};
}

bool relation::exists(const relations& r, const wstring& name) {
	return r.find(name) != r.end();
}

int relation::size(const relations& r, const wstring& name) {
	auto it = r.find(name);
	if (it == r.end()) return 0;
	return it->second->rows();
}

bool relation::nonempty(const relations& r, const wstring& name) {
	return relation::size(r, name) > 0;
}

ostream& operator<<(ostream& os, const relation& r) {
	if (!r.nr) return os << r.n << ".\n";
	for (int i = 0; i != r.nr; ++i) {
		os << r.n;
		if (r.nc) {
			os << "(";
			for (int j = 0; j != r.nc; ++j) os << (j?" ":"")
				<< query::stringify(r.t.at(i).at(j));
			os << ")";
		}
		os << ".\n";
	}
	return os;
}

ostream& operator<<(ostream& os, const relations& rels) {
	for (auto& it : rels) os << *(it.second);
	return os;
}

}
