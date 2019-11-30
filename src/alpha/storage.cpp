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
#include "storage.h"

namespace alpha {

using std::make_shared;
using std::vector;
template<typename T>
using sp = std::shared_ptr<T>;

template class storage<agent>;
template class storage<channel>;
template class storage<message>;

template<typename T>
typename storage<T>::item storage<T>::get(const unique_id& id) {
	if (id == "") return 0;
	auto it = data.find(id);
	if (it == data.end()) {
		auto ids = protocol::fetch<T>(sid, { id });
		// std::cout << "storage::get found n ids: " << ids.size() << std::endl;
		if (!ids.size()) return 0;
		data[id] = make_shared<T>(ids[0]);
		// std::cout << "\tfetching new object with id: " << ids[0] << std::endl;
		// console_log("fetching new"); //, data[id].get());
		return data[id];
	}
	// std::cout << "\tfetching ok" << std::endl;
	// console_log("fetching ok");
	return it->second;
}

template<typename T>
typename storage<T>::item storage<T>::get(const filter &f) {
	unique_ids ids = protocol::query<T>(sid, f);
	//log("ids got: ") << ids.size();
	if (!ids.size()) return 0;
	return get(ids[0]); // fetch only first item
}

template<typename T>
vector<typename storage<T>::item> storage<T>::get_list(const unique_ids &ids) {
	// std::cout << "storage::get_list " << ids << std::endl; //, data[id].get());
	if (!ids.size()) return {};
	//console_log("get "+id); //, data[id].get());
	unique_ids to_fetch{};
	vector<typename storage<T>::item> r(ids.size());
	size_t i = 0;
	for (auto &id : ids) {
		auto it = data.find(id);
		if (it == data.end()) r[i] = 0, to_fetch.push_back(id);
		else r[i] = it->second;
		++i;
	}
	if (!to_fetch.size()) return r;
	auto got = protocol::fetch<T>(sid, to_fetch);
	auto it = got.begin();
	for (auto rit = r.begin(); rit != r.end(); rit++) {
		if (it != got.end() && !(*rit).get()) {
			*rit = make_shared<T>(*it);
			++it;
		}
	}
	i = 0;
	for (auto e : r) {
		std::cout << i++ << " e: " << *e << std::endl;
	}
	return r;
}

template<typename T>
vector<typename storage<T>::item> storage<T>::get_list(const filter &f) {
	// std::cout << "storage::get_list with filter sid: "<<sid<< std::endl;
	unique_ids ids = protocol::query<T>(sid, f);
	// std::cout << "\tfound n ids: "<<ids.size()<< std::endl;
	if (!ids.size()) return {};
	return get_list(ids);
}

}
