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
using std::string;

namespace alpha::wt {

using agent_render_type = view::agent::render_type;


WString tr(string s) { return WString::tr(s); }

unique_ptr<WApplication> create_app(const WEnvironment& env) {
	return make_unique<app>(env);
}

unique_ptr<WWidget> app::profile() {
	auto up = make_unique<view::agent>(a.get(), agent_render_type::SHORT);
	profile_ = up.get();
	return up;
}

unique_ptr<WWidget> app::feed() {
	auto up = make_unique<view::channel>(channel{});
	feed_ = up.get();
	return up;
}

unique_ptr<WWidget> app::ide() {
	auto up = make_unique<ide::ide>();
	ide_ = up.get();
	return up;
}

int app::start(int argc, char** argv) {
	try {
		WServer server(argv[0]);
		server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
		auto ep_app = EntryPointType::Application;
		server.addEntryPoint(ep_app, &create_app);
		//server.addEntryPoint(ep_app, &create_some_app, "/some_app");
		if (server.start()) {
			int sig = WServer::waitForShutdown();
			std::cerr << "Shutdown (signal = " << sig << ")"
								<< std::endl;
			server.stop();
		}
	} catch (WServer::Exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	} catch (std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
		return 1;
	}
	return 0;
}

app::app(const WEnvironment& env) : WApplication(env) {

	// TODO register form (now just create user john)
	a = make_unique<agent>("john", "secret", "John Doe");
	a_ = a.get();
	protocol::register_agent(*a_);
	sid = protocol::login(a_->id, a_->password);
	console_log("session id: ", sid);

	const string *themePtr = env.getParameter("theme");
	string theme;
	if (!themePtr) theme = "bootstrap3";
	else theme = *themePtr;
	if (theme == "bootstrap3") {
		auto bootstrapTheme = std::make_shared<WBootstrapTheme>();
		bootstrapTheme->setVersion(BootstrapVersion::v3);
		bootstrapTheme->setResponsive(true);
		setTheme(bootstrapTheme);
		useStyleSheet(
			"resources/themes/bootstrap/3/bootstrap-theme.min.css");
	} else if (theme == "bootstrap2") {
		auto bootstrapTheme = std::make_shared<WBootstrapTheme>();
		bootstrapTheme->setResponsive(true);
		setTheme(bootstrapTheme);
	} else setTheme(std::make_shared<WCssTheme>(theme));

	create_ui();
}

WMenuItem* app::add_to_menu(WMenu *menu, const WString& name,
	unique_ptr<WWidget> c, unique_ptr<WWidget> t = 0)
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
	add_to_menu(menu.get(), tr("PROFILE"), profile());
	add_to_menu(menu.get(), tr("FEED"),    feed());
	add_to_menu(menu.get(), tr("IDE"),     ide());
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
