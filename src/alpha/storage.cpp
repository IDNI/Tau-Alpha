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

using std::vector;
template<typename T>
using sp = std::shared_ptr<T>;

template class storage<agent>;
template class storage<channel>;
template class storage<message>;

template<typename T>
vector<sp<T>> storage<T>::get(const unique_ids &ids) {
	if (!ids.size()) return {};
	//console_log("get "+id); //, data[id].get());
	unique_ids to_fetch{};
	vector<sp<T>> r(ids.size());
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
		auto &v = *rit;
		if (it != got.end() && !v.get())
			v = std::make_shared<T>(*it++);
	}
	return r;
}

template<typename T>
sp<T> storage<T>::get(const unique_id& id) {
	//console_log("get "+id); //, data[id].get());
	auto it = data.find(id);
	if (it == data.end()) {
		auto ids = protocol::fetch<T>(sid, { id });
		if (!ids.size()) return 0;
		data[id] = std::make_shared<T>(ids[0]);
		//console_log("fetching new"); //, data[id].get());
		return data[id];
	}
	//console_log("fetching ok");
	return it->second;
}

template<typename T>
sp<T> storage<T>::get(const filter &f) {
	unique_ids ids = protocol::query<T>(sid, f);
	//log("ids got: ") << ids.size();
	if (!ids.size()) return 0;
	return get(ids[0]); // fetch only first item
}

template<typename T>
vector<sp<T>> storage<T>::get_list(const filter &f) {
	unique_ids ids = protocol::query<T>(sid, f);
	//log("ids.size() ") << ids.size();
	if (!ids.size()) return {};
	return get(ids);
}

}
