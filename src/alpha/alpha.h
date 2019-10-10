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

namespace alpha {

Wt::WString tr(std::string s);

struct alpha : public Wt::WApplication {
	alpha(const Wt::WEnvironment& env);
	static int start(int argc, char** argv);
private:
	void create_views();
	void log(std::string message) { alpha::log("info", message); }
	void log(std::string level, std::string message) {
		Wt::log(level) << message;
	}
};

}
#endif
