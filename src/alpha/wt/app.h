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
#include <Wt/WMenu.h>

#include "../config.h"

#include "../storage.h"

#include "view/agent.h"
#include "view/channel.h"
#include "view/channels.h"
#include "view/message.h"

#include "ide/ide.h"

namespace alpha::wt {

using up_widget = std::unique_ptr<Wt::WWidget>;
using sp_widget = std::shared_ptr<Wt::WWidget>;

Wt::WString tr(std::string s);

class app : public Wt::WApplication {
	session_id sid;
	agent_id aid{};
	channel_id chid{};
	agent *agent_{0};
	channel aggregator_{"aggregator"};
	std::unique_ptr<storage<agent>> agents_;
	std::unique_ptr<storage<channel>> channels_;
	std::unique_ptr<storage<message>> messages_;

	Wt::WLogEntry log(std::string level, std::string message) {
		return Wt::log(level) << message;
	}
	Wt::WLogEntry log(std::string message) { return log("info", message); }
	void console_log(std::string level, std::string message);
	void console_log(std::string message) { console_log("info", message); }

	void create_ui();
	Wt::WMenu* menu_;
	Wt::WMenuItem* add_to_menu(Wt::WMenu *menu, const Wt::WString& name,
		up_widget w, up_widget t);
	up_widget view_registration();
	up_widget view_identities();
	up_widget view_channels();
	up_widget view_profile();
	up_widget view_ide();
	up_widget view_aggregator();
	up_widget view_channel(const channel_id& chid);
	up_widget view_channel_by_name(const std::string& n);

	view::agent *profile_;
	view::channels *channels_view;
	view::channel *channel_view;
public:
	static std::unique_ptr<config> cfg;

	app(const Wt::WEnvironment& env);
	static int start(int argc, char** argv);
	sp_agent get_agent(const agent_id& aid);
	session_id get_session_id() const { return sid; }
	storage<agent>* agents() const { return agents_.get(); };
	storage<message>* messages() const { return messages_.get(); };
	storage<channel>* channels() const { return channels_.get(); };
	static app* instance() {
		return dynamic_cast<app*>(Wt::WApplication::instance());
	};
};

}

#endif
