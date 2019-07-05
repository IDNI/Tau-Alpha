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

#include "../../submodules/TML/src/driver.h"
#include "splitjs.h"
#include "TML_editor.h"

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
	splitjs *sc_;
	TML_editor *editor_;
	TML_editor *output_;
	WTemplate * sb_;

	status status_ = INIT;
	bool changed_ = false;

	void create_menu();
	void create_toolbar();
	void create_splitter();
	void create_statusbar();

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
	void run_tml();
	void run_tml(std::string prog);
	void log(std::string message) { alpha::log("info", message); }
	void log(std::string level, std::string message);
};

}
#endif
