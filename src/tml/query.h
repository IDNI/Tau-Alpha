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
#ifndef __ALPHA_TML_QUERY_H__
#define __ALPHA_TML_QUERY_H__
#include <string>

namespace tml {

class datafile;
class relationfile;

class query {
	friend datafile;
	friend relationfile;
	static std::string insert_args__(const std::string& name, size_t n,
		va_list args);
	static std::string insert__(const std::string& rname, size_t n, ...);
	static std::string update__(const std::string& rname, int arity,
		std::string search, int col, size_t n, ...);
public:
	template<typename ...T>
	static std::string insert(const std::string& rname, T&&...entities) {
		return insert__(
			rname, sizeof...(entities), entities...);
	}
	template<typename ...T>
	static std::string update(const std::string& rname, int arity=0,
		std::string search="", int col=0, T&&...entities)
	{
		return update__(rname, arity, search, col,
			sizeof...(entities), entities...);
	}
	static std::string erase(const std::string& rname, int arity=0,
		std::string search="", int col=0);
	static std::string exists(const std::string& rname, int arity=0,
		std::string search="", int col=0);

	static std::string toggle_enabled(const std::string& ent, bool enable);
	static std::string toggle_selected(const std::string& ent,
		const std::string&o, bool select=true);
	static std::string select(const std::string& n, const std::string& o) {
					return toggle_selected(n, o); }
	static std::string unselect(const std::string& n, const std::string& o){
					return toggle_selected(n, o, false); }

	static std::string escape(const std::string& unescaped);
	static std::string stringify(const std::string& s);
};

}

#endif
