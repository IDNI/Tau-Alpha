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
#include "config.h"
#include "../tml/query.h"

namespace alpha {

using std::wstring;
using std::string;

config::config(const string& df, const string& pf) : tml::datafile(df, pf) {
	args.push_back("--input"), args.push_back("config/config.common.ptml");
	cached_args = cache_args();
	r = process(cached_args);
}
config::config(const string& df) : config(df, "") {}

bool config::enabled(const string& name) {
	return get_relation("enabled").find_row(name) != -1;
	//return datafile::exists("enabled", 1, name);
}

void config::enable(const string& name) {
	set_enabled(name);
}

void config::disable(const string& name) {
	set_enabled(name, false);
}

void config::set_enabled(const string& name, bool enabled) {
	update(tml::query::toggle_enabled(name, enabled));
}

std::string config::selected(const std::string& name) {
	tml::relation r_selected = get_relation("selected");
	int row_n = r_selected.find_row(name);
	if (row_n == -1) return "";
	tml::row& row = r_selected.get_row(row_n);
	if (!row.size()) return "";
	return row[1];
}

bool config::selected(const std::string& name, const std::string& option) {
	return option == selected(name);
}

void config::select(const std::string& name, const std::string& option) {
	update(tml::query::select(name, option));
}

void config::unselect(const std::string& name, const std::string& option) {
	update(tml::query::unselect(name, option));
}


}
