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
#ifndef __ALPHA_TML_RELATION_H__
#define __ALPHA_TML_RELATION_H__

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace tml {

class relation;
using relations = std::map<std::wstring, std::shared_ptr<relation>>;
using strings = std::vector<std::string>;
using column = strings;
using row = strings;
using table = std::vector<row>;

class relation {
	friend std::ostream& operator<<(std::ostream& os, const relation& r);
	bool valid_{false};
	std::string n = "";
	int nr{0};
	int nc{0};
	table t{};
public:
	relation() = default;
	relation(std::string n);
	bool valid() const { return valid_; }
	bool invalid() const { return !valid(); }
	std::string name() const { return n; }
	int columns() const { return nc; };
	int rows() const { return nr; };
	bool row_exist(int row) const;
	int find_row(std::string search, int col = 0) const;
	row& get_row(int row);
	column get_column(int col);
	table get_table() { return t; };
	std::string get(int row, int column) const;
	std::string &set(int row, int column, std::string value);
	static relation find(const relations& rels, const std::wstring& name);
	static bool exists(const relations& rels, const std::wstring& name);
	static int size(const relations& rels, const std::wstring& name);
	static bool nonempty(const relations& rels, const std::wstring& name);
};

std::ostream& operator<<(std::ostream& os, const relation& r);
std::ostream& operator<<(std::ostream& os, const relations& rels);

}

#endif
