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
#ifndef __ALPHA_CONFIG_H__
#define __ALPHA_CONFIG_H__

#include "../tml/datafile.h"

namespace alpha {

class config : public tml::datafile {
public:
	config(const std::string& df);
	config(const std::string& df, const std::string& pf);

	// enabled(name)
	bool enabled(const std::string& name);
	void enable(const std::string& name);
	void disable(const std::string& name);
	void set_enabled(const std::string& name, bool enabled = true);
	bool disabled(const std::string& name) { return !enabled(name); };

	// selected(name, selected_option)
	bool selected(const std::string& name, const std::string& option);
	std::string selected(const std::string& name);
	void select(const std::string& name, const std::string& option);
	void unselect(const std::string& name, const std::string& option);
	bool unselected(const std::string& name, const std::string& option) {
					return !selected(name, option); };

	// wstring alternatives
	bool enabled(const std::wstring& name) { return enabled(ws2s(name)); }
	void enable(const std::wstring& name) { return enable(ws2s(name)); }
	void disable(const std::wstring& name) { return disable(ws2s(name)); }
	void set_enabled(const std::wstring& name, bool enabled) {
					set_enabled(ws2s(name), enabled); }
	bool disabled(const std::wstring& name) { return !enabled(name); };
	bool selected(const std::wstring& name, const std::wstring& option) {
				return selected(ws2s(name), ws2s(option)); }
	std::wstring selected(const std::wstring& name) {
				return s2ws(selected(ws2s(name))); }
	void select(const std::wstring& name, const std::wstring& option) {
				return select(ws2s(name), ws2s(option)); }
	void unselect(const std::wstring& name, const std::wstring& option) {
				return unselect(ws2s(name), ws2s(option)); }
	bool unselected(const std::wstring& name, const std::wstring& option) {
					return !selected(name, option); };

};

}

#endif
