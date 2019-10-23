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
#ifndef __ALPHA_WT_FILE_ITEM_H__
#define __ALPHA_WT_FILE_ITEM_H__

#include <string.h>

#include <Wt/WStandardItem.h>
#include <Wt/WString.h>

#include <boost/filesystem.hpp>

namespace alpha::wt {

class file_item : public Wt::WStandardItem {
public:
	static const Wt::ItemDataRole FileNameRole;
	static const Wt::ItemDataRole FilePathRole;

	file_item(const std::string& iconUri, const Wt::WString& text,
		const std::string& fileName) : Wt::WStandardItem(iconUri, text)
	{
		setData(text, FileNameRole);
		setData(fileName, FilePathRole);
	}
};

}
#endif
