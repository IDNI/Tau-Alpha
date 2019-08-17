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
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include <boost/filesystem.hpp>

#include "workspace.h"
#include "file_item.h"

using namespace std;
using namespace Wt;
namespace fs = boost::filesystem;

static bool comparePaths(const fs::path& p1, const fs::path& p2) {
	return p1.filename() > p2.filename();
}

string readFileToString(const string& fileName)
{
	size_t outputFileSize = (size_t)fs::file_size(fileName);
	fstream file (fileName.c_str(), ios::in | ios::binary);
	char* memblock = new char [outputFileSize];
	file.read(memblock, (streamsize)outputFileSize);
	file.close();
	string data = string(memblock, outputFileSize);
	delete [] memblock;
	return data;
}

workspace::workspace(const string& workspaceRoot) :
	workspaceRoot_(workspaceRoot) {}

void workspace::setFile(const string& dir, const string& file) {
	clear();
	bool exists = false;
	try {
		exists = fs::exists(dir);
	} catch (exception&) {
		throw 0;
	}

	if (!exists) {
		addWidget(make_unique<WText>("No such program: " + dir));
		return;
	}

	model_ = make_shared<WStandardItemModel>(0, 1);
	traverseDir(model_->invisibleRootItem(), dir);

	auto view = make_unique<WTreeView>();
	view_ = view.get();
	view_->setHeaderHeight(0);
	view_->resize(300, WLength::Auto);
	view_->setSortingEnabled(false);
	view_->setModel(model_);
	view_->expandToDepth(1);
	view_->setSelectionMode(SelectionMode::Single);
	view_->setAlternatingRowColors(false);
	view_->selectionChanged().connect(this, &workspace::showFile);

	WStandardItem *w = model_->item(0);
	do {
		view_->setExpanded(w->index(), true);
		if (w->rowCount() > 0)
			w = w->child(0);
		else {
			view_->select(w->index());
			w = 0;
		}
	} while (w);

	auto layout = make_unique<WVBoxLayout>();
	title_ = layout->addWidget(make_unique<WText>(""));
	title_->addStyleClass("workspace_title");
	layout->addWidget(move(view), 1);
	setLayout(move(layout));
	setStyleClass("workspace");
	setTitle(file);
}

void workspace::setTitle(const WString &t) {
	WApplication::instance()->setTitle(tr("Tau Alpha") + " - " + t);
	if (title_) title_->setText(t);
}

void workspace::traverseDir(WStandardItem* parent, const fs::path& path) {
	static const char *supportedFiles[] = { ".tml", ".g", 0 };

	auto dir = make_unique<file_item>("/icons/yellow-folder-open.png",
		path.filename().string(), "");
	file_item *dirPtr = dir.get();
	parent->appendRow(move(dir));
	parent = dirPtr;
	try {
		set<fs::path> paths;

		fs::directory_iterator end_itr;
		for (fs::directory_iterator i(path); i != end_itr; ++i)
			paths.insert(*i);

		vector<unique_ptr<file_item>> files;
		vector<fs::path> dirs;

		while (!paths.empty()) {
			fs::path p = *paths.begin();
			paths.erase(p);

			if (fs::is_symlink(p)) continue;
			if (fs::is_regular(p)) {
				string ext = fs::extension(p);
				bool supported = false;
				for (const char **s = supportedFiles; *s != 0; ++s)
					if (*s == ext) {
						supported = true;
						break;
					}

				if (!supported) continue;
			}

			if (fs::is_directory(p)) {
				dirs.push_back(p);
			} else {
				auto file = make_unique<file_item>(
						"/icons/document.png",
						p.filename().string(),
						p.string());
				files.push_back(move(file));
			}
		}

		sort(dirs.begin(), dirs.end(), comparePaths);

		for (unsigned int i = 0; i < files.size(); i++)
			parent->appendRow(move(files[i]));

		for (unsigned int i = 0; i < dirs.size(); i++)
			traverseDir(parent, dirs[i]);
	} catch (fs::filesystem_error& e) {
		cerr << e.what() << endl;
	}
}

void workspace::showFile() {
	if (view_->selectedIndexes().empty()) return;

	selected_ = *view_->selectedIndexes().begin();

	if (view_->model()->rowCount(selected_) > 0
	&& !view_->isExpanded(selected_))
		view_->setExpanded(selected_, true);

	cpp17::any fileNameData = selected_.data(file_item::FileNameRole);
	string fileName = asString(fileNameData).toUTF8();
	cpp17::any filePathData = selected_.data(file_item::FilePathRole);
	string filePath;
	if (cpp17::any_has_value(filePathData)) filePath = asString(filePathData).toUTF8();

	if (filePath != "" && editor_) {
		editor_->setText(readFileToString(filePath));
		if (fileName != "") {
			WString t = tr("title." + fileName);
			setTitle(t == "??title."+fileName+"??" ? fileName : t);
		}
	}
	if (onShowFile_) onShowFile_();
}
