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
#ifndef __ALPHA_WT_IDE_IDE_H__
#define __ALPHA_WT_IDE_IDE_H__
#include <string>

#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WJavaScript.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WString.h>
#include <Wt/WTable.h>
#include <Wt/WTabWidget.h>
#include <Wt/WTemplate.h>
#include <Wt/WToolBar.h>
#include <Wt/WText.h>

#include "../splitjs.h"
#include "../TML_editor.h"
#include "../workspace.h"

#include "menu.h"
#include "toolbar.h"

namespace alpha::wt::ide {

typedef enum status_values { ERROR,
	INIT, LOADED, RUNNING, STOPPED, FINISHED, UNSAT, CONTRADICTION
} status;

std::string bin2hex(const std::string& bin);

struct ide : public Wt::WTemplate {
	ide();
	void render();
	static std::map<status_values, std::string> status_name;
private:
	struct opts {
		bool autorun = false;
		bool linenumbers = true;
	} o;

	status status_ = INIT;
	bool changed_ = false;      // has source been changed since last run?

	menu* menu_;
	toolbar* toolbar_;
	splitjs* ui_;
	Wt::WTemplate* statusbar_;
	splitjs* iui_;
	workspace *workspace_;
	TML_editor *editor_;            // editor
	Wt::WTabWidget *tabs_;          // tabs bellow editor
	TML_editor *output_;            // output tab
	Wt::WContainerWidget *errors_;  // errors tab
	Wt::WContainerWidget *info_;    // info tab
	Wt::WContainerWidget *debug_;   // debug tab
	Wt::WContainerWidget *tabular_; // tabular tab
	Wt::WTabWidget *tabular_tabs_;  // tabular tabs with out tables
	std::map<std::wstring, Wt::WTable*> tables_;
	Wt::WContainerWidget *binary_;  // binary tab

	Wt::WPushButton *add_btn_;

	Wt::JSignal<bool, double, std::string> result_;
	Wt::JSignal<std::string> relation_ensure_;
	Wt::JSignal<std::string, int, std::string> relation_set_;
	Wt::JSignal<>output_finished_;
	Wt::JSignal<std::string, std::string> set_tab_text_;

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

	// UI updates
	void elapsed(double ms) {
		std::stringstream ss; ss << ms << " ms";
		statusbar_->bindString("elapsed", ss.str());
	}
	void changed() {
		changed_ = true; statusbar_->bindString("changed", "*");
	}
	void not_changed() {
		changed_ = false; statusbar_->bindString("changed", "");
	}
	void set_autorun(bool a) { o.autorun = a; };
	void update_status(status s = ERROR) {
		status_ = s;
		statusbar_->bindString("status", tr(status_name[status_]));
		statusbar_->bindInt("elapsed", 0);
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
