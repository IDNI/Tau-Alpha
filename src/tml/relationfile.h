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
#ifndef __ALPHA_TML_RELATIONFILE_H__
#define __ALPHA_TML_RELATIONFILE_H__
#include <stdarg.h>
#include <TML.h>

#include "datafile.h"

namespace tml {

class relationfile : public datafile {
protected:
	std::string rn{};
	int arity{0};
	void insert__(size_t n, ...);
public:
	relationfile(const std::string& rn, int arity, const std::string& dfile,
		const std::string& pfile, strings args={});
	relationfile(const std::string& rn, int arity, const std::string& dfile,
		strings args={});

	relation get_relation() { return datafile::get_relation(rn); }
	int rows() { return datafile::size(rn); };
	int size() { return rows(); };
	bool nonempty() { return rows() > 0;};

	// operations
	int max(int col=0, int min=0);
	row find(std::string search="", int col=0);
	bool exists(std::string search="", int col=0);
	void erase(std::string search="", int col=0);
	template<typename ...T> void insert(T&&...entities) {
		insert__(sizeof...(entities), entities...);
	}
};

}

#endif
