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
#include "../protocol.h"

using namespace Wt;
using std::unique_ptr;
using std::make_unique;
using std::make_shared;
using std::string;
using std::cerr;
using std::endl;

namespace alpha::wt {

using agent_rt   = view::agent::render_type;
using message_rt = view::message::render_type;

WString tr(string s) { return WString::tr(s); }

unique_ptr<WApplication> create_app(const WEnvironment& env) {
	return make_unique<app>(env);
}

up_widget app::view_profile() {
	return make_unique<view::agent>(agent_rt::SHORT, user_);
}

up_widget app::view_channel(const channel_id& chid = channel_id{}) {
	return make_unique<view::channel>(chid == channel_id{}
		? aggr_ // use aggregator feed if no chid
		: channels()->get(chid));
}

up_widget app::view_channel_by_name(const string& name = "") {
	sp_channel& ch = aggr_;
	if (name != "") {
		channel_f f{}; f.name = name;
		ch = channels()->get(f);
	}
	return make_unique<view::channel>(ch);
}

up_widget app::view_ide() {
	return make_unique<ide::ide>();
}

int app::start(int argc, char** argv) {
	try {
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
	// register and login user john TODO: register form
	user_ = make_shared<agent>("john", "secret", "John Doe");
	auto a = user_.get();
	bool r = protocol::register_agent(*a);       // register
	if (!r) a->id = "1";                         // or set agents id
	sid = protocol::login(a->id, a->password);
	log("session id: ", sid);

	// storages for network objects
	agents_   = make_unique<storage<agent>>(sid);
	channels_ = make_unique<storage<channel>>(sid);
	messages_ = make_unique<storage<message>>(sid);

	// aggregator channel
	aggr_ = make_shared<channel>("aggr");

	// create testing channels
	channel ch1 = channel("channel1");
	protocol::create_channel(sid, ch1);
	channel ch2 = channel("channel2");
	protocol::create_channel(sid, ch2);

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
	add_to_menu(menu.get(), tr("PROFILE"), view_profile());
	// add_to_menu(menu.get(), tr("FEED"), view_channel());
	add_to_menu(menu.get(), tr("CHANNEL")+"1",
					view_channel_by_name("channel1"));
	add_to_menu(menu.get(), tr("CHANNEL")+"2",
					view_channel_by_name("channel2"));
	// add_to_menu(menu.get(), tr("CHANNEL")+"3",
	// 				view_channel_by_name("channel3"));
	add_to_menu(menu.get(), tr("IDE"), view_ide());
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
