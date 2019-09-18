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
#ifndef __WT_ALPHA_H__
#define __WT_ALPHA_H__

#include <string>

#include <Wt/WApplication.h>
#include <Wt/WString.h>
#include <Wt/WTemplate.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WToolBar.h>
#include <Wt/WJavaScript.h>
#include <Wt/WTabWidget.h>
#include <Wt/WTable.h>
#include <TML.h>

#include "splitjs.h"
#include "TML_editor.h"
#include "workspace.h"

Wt::WString tr(std::string s);
typedef enum status_values { ERROR,
	INIT, LOADED, RUNNING, STOPPED, FINISHED, UNSAT, CONTRADICTION
} status;
extern std::map<status_values, std::string> status_name;

namespace Wt {

struct alpha : public WApplication {
	alpha(const WEnvironment& env);
	static int run(int argc, char** argv);
private:
	struct opts {
		bool autorun = false;
		bool linenumbers = true;
	} o;
	WContainerWidget *c_;
	workspace *workspace_;
	splitjs *wsc_;              // splits file tree and right part of UI
	splitjs *sc_;               // splits editor and tabs under editor
	TML_editor *editor_;        // editor
	WTabWidget *tabs_;          // tabs under editor
	TML_editor *output_;        // output tab
	WContainerWidget *errors_;  // errors tab
	WContainerWidget *info_;    // info tab
	WContainerWidget *debug_;   // debug tab
	WContainerWidget *tabular_; // tabular tab
	WTabWidget *tabular_tabs_;  // tabular tabs with out tables
	std::map<std::wstring, WTable*> tables_;
	//WContainerWidget *binary_;  // binary tab
	WTemplate *sb_;             // status bar

	WPushButton *run_btn_;
	WPushButton *runjs_btn_;
	WPushButton *add_btn_;

	JSignal<bool, double> result_;

	status status_ = INIT;
	bool changed_ = false;      // has source been changed since last run?

	void load_tml_js();
	void create_menu();
	void create_toolbar();
	void create_splitters_ui();
	void create_statusbar();

	void before_run();
	void run_tml();
	void run_tml(std::string prog);
	void run_tml_js();

	void log(std::string message) { alpha::log("info", message); }
	void log(std::string level, std::string message);

	// UI updates
	void elapsed(double ms) {
		std::stringstream ss; ss << ms << " ms";
		sb_->bindString("elapsed", ss.str());
	}
	void changed() { changed_=true; sb_->bindString("changed", "*"); }
	void not_changed() { changed_=false; sb_->bindString("changed", ""); }
	void set_autorun(bool a) { o.autorun = a; };
	void update_status(status s = ERROR) {
		status_ = s;
		sb_->bindString("status", tr(status_name[status_]));
		sb_->bindInt("elapsed", 0);
	}

	// helpers
	void add_text(WContainerWidget* w, const std::wstring& text);
	void add_text(WContainerWidget* w, const std::string& text);
	//std::string serialize_result(driver &d);
	//void unserialize_result(driver &d, std::string bin);
};

}
#endif
