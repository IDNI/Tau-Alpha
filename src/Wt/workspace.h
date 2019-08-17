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
#ifndef __WT_WORKSPACE_H__
#define __WT_WORKSPACE_H__
#include <iostream>
#include <stdlib.h>
#include <functional>

#include <Wt/WContainerWidget.h>
#include <Wt/WTreeView.h>
#include <Wt/WStandardItemModel.h>

#include <TML.h>

#include "TML_editor.h"
#include "file_item.h"

namespace Wt {

class workspace : public WContainerWidget {
	WTreeView  *view_;
	WText *title_ = 0;
	TML_editor *editor_ = 0;
	WModelIndex selected_;
	std::string workspaceRoot_;
	std::shared_ptr<WStandardItemModel> model_;
	std::function<void()> onShowFile_;

	void traverseDir(WStandardItem* parent,
		const boost::filesystem::path& path);
	void showFile();
	void handlePathChange();
public:
	workspace(const std::string& workspaceRoot);
	void setEditor(TML_editor *editor) { editor_ = editor; };
	void setFile(const std::string& dir, const std::string& file);
	void setTitle(const WString& t);
	void onShowFile(std::function<void()> fn) { onShowFile_ = fn; };
};

}

#endif
