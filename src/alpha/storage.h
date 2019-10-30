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
#ifndef __ALPHA_STORAGE_H__
#define __ALPHA_STORAGE_H__

#include "defs.h"
#include "protocol.h"

namespace alpha {

template<typename T>
class storage {
	using item = std::shared_ptr<T>;
	session_id sid;
	std::map<unique_id, item> data{};
public:
	storage(const session_id& sid) : sid(sid) {}
	item get(const unique_id& id);
	item get(const filter &f);
	std::vector<item> get_list(const unique_ids &ids);
	std::vector<item> get_list(const filter &f); //size_t /*limit = 0*/);
};

}

#endif
