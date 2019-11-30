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
#ifndef __ALPHA_TML_DATAFILE_H__
#define __ALPHA_TML_DATAFILE_H__
#include <stdarg.h>
#include <TML.h>

#include "relation.h"

namespace tml {

class datafile {
protected:
	std::string dfile{""}; // data file (updatable facts)
	std::string pfile{""}; // program file (rules and constants)
	strings args{};
	strings cached_args{};
	relations r{};
	strings cache_args();
	void insert__(const std::string& rname, size_t n, ...);
public:
	datafile(const std::string& dfile, const std::string& pfile,
		strings args={});
	datafile(const std::string& dfile, strings args={});

	static void init();

	void set_args(strings n_args) { args=n_args; cached_args=cache_args(); }

	static datafile load(const std::string& dfile, const std::string& pfile,
		strings args={});
	static datafile load(const std::string& dfile, strings args={});

	void load();
	void save();
	void save(relations rels);
	relations run(const std::string& e); // do not change internal relations
	relations eval(const std::string& e); // updates internal relations
	relations update(const std::string& e); // updates and saves to file too

	static relations process(strings args, bool dontload = false);
	static void save(const std::string& f, const relations& rels={});

	// relations by n
	relation get_relation(const std::wstring& n) { return
							relation::find(r, n); }
	bool exists(const std::wstring& n) { return relation::exists(r, n); };
	int size(const std::wstring& n) { return relation::size(r, n); };
	bool nonempty(const std::wstring& n) { return relation::nonempty(r,n);};

	relation get_relation(const std::string& n) {
		return get_relation(s2ws(n)); }
	bool exists(const std::string& n) { return exists(s2ws(n)); };
	int size(const std::string& n) { return size(s2ws(n)); };
	bool nonempty(const std::string& n) { return nonempty(s2ws(n)); };

	// operations
	int max(const std::string& rname, int col=0, int min=0);
	row find(const std::string& rname, std::string search="",
		int col=0);
	bool exists(const std::string&rname, int arity=0, std::string search="",
		int col=0);
	void erase(const std::string& rname, int arity=0, std::string search="",
		int col=0);
	template<typename ...T>
	void insert(const std::string& rname, T&&...entities) {
		insert__(rname, sizeof...(entities), entities...);
	}
};

}

#endif
