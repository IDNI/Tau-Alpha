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
#ifndef __ALPHA_H__
#define __ALPHA_H__

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

#include "widget/splitjs.h"
#include "widget/TML_editor.h"
#include "widget/workspace.h"
#include "driver.h"

Wt::WString tr(std::string s);
typedef enum status_values { ERROR,
	INIT, LOADED, RUNNING, STOPPED, FINISHED, UNSAT, CONTRADICTION
} status;
extern std::map<status_values, std::string> status_name;

namespace alpha {

std::string bin2hex(const std::string& bin);

struct alpha : public Wt::WApplication {
	alpha(const Wt::WEnvironment& env);
	static int start(int argc, char** argv);
private:
	struct opts {
		bool autorun = false;
		bool linenumbers = true;
	} o;

	status status_ = INIT;
	bool changed_ = false;      // has source been changed since last run?

	Wt::WContainerWidget *c_;
	widget::workspace *workspace_;
	widget::splitjs *wsc_;          // splits file tree and right part of UI
	widget::splitjs *sc_;           // splits editor and tabs under editor
	widget::TML_editor *editor_;    // editor
	Wt::WTabWidget *tabs_;          // tabs under editor
	widget::TML_editor *output_;    // output tab
	Wt::WContainerWidget *errors_;  // errors tab
	Wt::WContainerWidget *info_;    // info tab
	Wt::WContainerWidget *debug_;   // debug tab
	Wt::WContainerWidget *tabular_; // tabular tab
	Wt::WTabWidget *tabular_tabs_;  // tabular tabs with out tables
	std::map<std::wstring, Wt::WTable*> tables_;
	Wt::WContainerWidget *binary_;  // binary tab
	Wt::WTemplate *sb_;             // status bar

	Wt::WPushButton *run_backend_btn_;
	Wt::WPushButton *run_frontend_btn_;
	Wt::WPushButton *add_btn_;

	Wt::JSignal<bool, double, std::string> result_;
	Wt::JSignal<std::string> relation_ensure_;
	Wt::JSignal<std::string, int, std::string> relation_set_;
	Wt::JSignal<>output_finished_;
	Wt::JSignal<std::string, std::string> set_tab_text_;

	void create_menu();
	void create_toolbar();
	void create_splitters_ui();
	void create_statusbar();

	void runtime_clear();
	void runtime_frontend_load();
	void runtime_before() {
		runtime_clear();
		update_status(RUNNING);
		not_changed();
	};
	void runtime_frontend();
	void runtime_backend() { runtime_backend(editor_->getText()); }
	void runtime_backend(std::string prog);
	void runtime_after() { refresh_tabs(); };

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
	void refresh_tabs();

	// helpers
	void add_text(Wt::WContainerWidget* w, const std::wstring& text);
	void add_text(Wt::WContainerWidget* w, const std::string& text);
	std::string serialize_result(driver &d);
	void unserialize_result(driver &d, std::string bin);
	Wt::WTable* get_table(std::wstring r);
};

}
#endif
