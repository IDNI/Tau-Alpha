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
#ifndef __ALPHA_WT_APP_H__
#define __ALPHA_WT_APP_H__

#include <string>

#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WString.h>
#include <Wt/WText.h>

#include "view/agent.h"
#include "view/channel.h"
#include "view/message.h"

#include "ide/ide.h"

namespace alpha::wt {

Wt::WString tr(std::string s);

struct app : public Wt::WApplication {
	app(const Wt::WEnvironment& env);
	static int start(int argc, char** argv);
private:
	session_id sid;
	std::unique_ptr<agent> a;
	agent* a_;

	view::agent* profile_;
	view::channel* feed_;
	ide::ide* ide_;

	void log(std::string level, std::string message) {
		Wt::log(level) << message;
	}
	void log(std::string message) { log("info", message); }
	void console_log(std::string level, std::string message);
	void console_log(std::string message) { console_log("info", message); }

	void create_ui();
	Wt::WMenuItem* add_to_menu(Wt::WMenu *menu, const Wt::WString& name,
		std::unique_ptr<Wt::WWidget> w, std::unique_ptr<Wt::WWidget> t);
	std::unique_ptr<Wt::WWidget> profile();
	std::unique_ptr<Wt::WWidget> feed();
	std::unique_ptr<Wt::WWidget> ide();
};

}

#endif
