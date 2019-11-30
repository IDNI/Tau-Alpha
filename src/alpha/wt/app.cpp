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
#include <string>
#include <iomanip>
#include <sstream>

#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WServer.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WNavigationBar.h>
#include <Wt/WMenu.h>

#include <Wt/WBootstrapTheme.h>
#include <Wt/WCssTheme.h>

#include "app.h"
#include "view/agent.h"
#include "view/message.h"
#include "view/channel.h"
#include "view/register_form.h"
#include "view/identities.h"
#include "view/channels.h"
#include "../protocol.h"

#include "../config.h"

#include "../../tml/relationfile.h"

namespace alpha::wt {

using namespace Wt;
using std::unique_ptr;
using std::make_unique;
using std::make_shared;
using std::wstring;
using std::string;
using std::cerr;
using std::endl;

using agent_rt   = view::agent::render_type;
using message_rt = view::message::render_type;

std::unique_ptr<config> app::cfg = 0;

WString tr(string s) { return WString::tr(s); }

unique_ptr<WApplication> create_app(const WEnvironment& env) {
	return make_unique<app>(env);
}

up_widget app::view_identities() {
	auto up = make_unique<view::identities>(
		tml::relationfile("identity", 3, "storage/identity.tml"),
		[this](session_id newsid, agent_id newaid){
			sid = newsid;
			aid = newaid;
			log("info")<<" new session: "<<sid<<" user: "<<aid;
			// console_log("new_session: "+sid+" user: "+aid);

			agents_  ->set_sid(sid);
			channels_->set_sid(sid);
			messages_->set_sid(sid);

			agent_ = agents()->get(aid).get();
			profile_->set_agent(agent_);

			channel_id chid = channels_view->get_channel_id();
			auto ch = channels()->get(chid);
			channel_view->set_channel(ch.get());
			channels_view->update_list();
			app::cfg->select("identity", aid);
		}
	);
	up->set_agent(app::cfg->selected("identity"));
	return up;
}

up_widget app::view_channels() {
	chid = app::cfg->selected("channel");
	auto on_update = [this](channel_id newchid) {
		chid = newchid;
		// log("info")<<" view channel: "<<chid;
		// console_log("view channel: "+chid);
		channel_view->set_channel(channels()->get(chid).get());
		app::cfg->select("channel", newchid);
		menu_->select(5);
	};
	auto up = make_unique<view::channels>(chid, on_update, on_update);
	channels_view = up.get();
	return up;
}

up_widget app::view_registration() {
	return make_unique<view::register_form>();
}

up_widget app::view_profile() {
	agent_ = agents()->get(aid).get();
	auto profile = make_unique<view::agent>(agent_rt::DETAIL, agent_);
	profile_ = profile.get();
	return profile;
}

up_widget app::view_aggregator() {
	return make_unique<view::channel>(&aggregator_, false);
}


up_widget app::view_channel(const channel_id& chid = channel_id{}) {
	auto up = make_unique<view::channel>(channels()->get(chid).get());
	channel_view = up.get();
	return up;
}

up_widget app::view_channel_by_name(const string& name = "") {
	if (name != "") return 0;
	channel_f f{}; f.name = name;
	alpha::channel* ch = channels()->get(f).get();
	if (!ch) return 0;
	auto up = make_unique<view::channel>(ch);
	channel_view = up.get();
	return up;
}

up_widget app::view_ide() {
	return make_unique<ide::ide>();
}

int app::start(int argc, char** argv) {
	try {
		if (!app::cfg.get()) app::cfg = make_unique<config>(
			"config/alpha.dtml", "config/alpha.ptml");
		app::cfg->load();

		protocol::init();

		WServer server(argv[0]);
		server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
		server.addEntryPoint(EntryPointType::Application, &create_app);
		if (server.start()) {
			int sig = WServer::waitForShutdown();
			cerr << "shutdown (signal = " << sig << ")" << endl;
			server.stop();
		}
		return 0;
	} catch (WServer::Exception& e) {
		cerr << e.what() << endl;
	} catch (std::exception& e) {
		cerr << "exception: " << e.what() << endl;
	}
	return 1;
}

app::app(const WEnvironment& env) : WApplication(env) {

	// storages for network objects
	agents_   = make_unique<storage<agent>>(sid);
	channels_ = make_unique<storage<channel>>(sid);
	messages_ = make_unique<storage<message>>(sid);

	// create notification for channel1 and channel2
	// notification n1; n1.targets = { ch1.id };
	// protocol::notify(sid, n1, [this](message_ids ids) {
	// 	log("on_notify 1: ")<<ids[0];
	// });
	// notification n2; n2.targets = { ch2.id };
	// protocol::notify(sid, n2, [this](message_ids ids) {
	//	log("on_notify 2: ")<<ids[0];
	// });

	// set boostrap theme
	const string *themePtr = env.getParameter("theme");
	string theme;
	if (!themePtr) theme = "bootstrap3";
	else theme = *themePtr;
	if (theme == "bootstrap3") {
		auto bootstrapTheme = make_shared<WBootstrapTheme>();
		bootstrapTheme->setVersion(BootstrapVersion::v3);
		bootstrapTheme->setResponsive(true);
		setTheme(bootstrapTheme);
		useStyleSheet(
			"resources/themes/bootstrap/3/bootstrap-theme.min.css");
	} else if (theme == "bootstrap2") {
		auto bootstrapTheme = make_shared<WBootstrapTheme>();
		bootstrapTheme->setResponsive(true);
		setTheme(bootstrapTheme);
	} else setTheme(make_shared<WCssTheme>(theme));

	create_ui();
}

WMenuItem* app::add_to_menu(WMenu *menu, const WString& name,
	up_widget c, up_widget t = 0)
{
	auto pane = make_unique<WContainerWidget>();
	auto pane_ = pane.get();
	auto hL = pane_->setLayout(make_unique<WHBoxLayout>());
	hL->setContentsMargins(0, 0, 0, 0);
	if (t) hL->addWidget(move(t));
	hL->addWidget(move(c));

	auto item = make_unique<WMenuItem>(name, move(pane));
	auto item_ = menu->addItem(move(item));
	return item_;
}

void app::create_ui() {
	messageResourceBundle().use("messages/alpha", false);
	setTitle(tr("Tau Alpha"));

	auto hL = root()->setLayout(make_unique<WHBoxLayout>());
  	hL->setContentsMargins(0, 0, 0, 0);

	auto div = hL->addWidget(make_unique<WContainerWidget>());
	auto vL = div->setLayout(make_unique<WVBoxLayout>());
	vL->setContentsMargins(0, 0, 0, 0);

	auto navui = make_unique<WContainerWidget>();

	auto navbar = navui->addNew<WNavigationBar>();
	navbar->setTitle(tr("Tau Alpha"), "/");
	navbar->setResponsive(true);
	auto contents = make_unique<WStackedWidget>();
	contents->addStyleClass("contents");
	auto menu = make_unique<WMenu>(contents.get());
	//menu->setInternalPathEnabled();
	//menu->setInternalBasePath("/");
	menu_ = menu.get();

	add_to_menu(menu_, tr("REGISTRATION"), view_registration());   // 0
	auto w_profile = view_profile();
	auto w_aggregator = view_aggregator();
	auto w_channel = view_channel();
	auto w_channels = view_channels();
	add_to_menu(menu_, tr("IDENTITY"), view_identities());         // 1
	add_to_menu(menu_, tr("PROFILE"), move(w_profile));            // 2
	add_to_menu(menu_, tr("CHANNELS"), move(w_channels));          // 3
	add_to_menu(menu_, tr("FEED"), move(w_aggregator));            // 4
	add_to_menu(menu_, tr("CHANNEL"), move(w_channel));            // 5
	if (app::cfg->enabled("IDE"))
		add_to_menu(menu_, tr("IDE"), view_ide());             // 6
	//menu_->itemSelectRendered().connect(this, &Home::updateTitle);

	navbar->addMenu(move(menu));
	vL->addWidget(move(navui), 0);
	vL->addWidget(move(contents), 1);

	useStyleSheet("alpha.css");
}

void app::console_log(string level, string value) {
	std::stringstream js; js
	<< "console.log(`" << level << "\n`, "
	<< std::quoted(value, '`')
	<< ");";
	doJavaScript(js.str());
}

}
