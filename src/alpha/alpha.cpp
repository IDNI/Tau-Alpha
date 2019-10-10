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
//#include <iomanip>
#include <Wt/WServer.h>
#include <Wt/WApplication.h>
#include <Wt/WTabWidget.h>
#include <Wt/WEnvironment.h>
#include <Wt/WText.h>
#include <Wt/WStackedWidget.h>
#include "alpha.h"

#include "widget/ide.h"
#include "widget/agent_view.h"
#include "widget/message_view.h"
#include "widget/channel_view.h"

namespace alpha {

Wt::WString tr(std::string s) { return Wt::WString::tr(s); }

using namespace Wt;
using std::unique_ptr;
using std::make_unique;
using std::string;

unique_ptr<WApplication> create_alpha(const WEnvironment& env) {
	return make_unique<alpha>(env);
}

unique_ptr<WApplication> create_ide(const WEnvironment& env) {
	return make_unique<ide>(env);
}

int alpha::start(int argc, char** argv) {
	try {
		WServer server(argv[0]);
		server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
		server.addEntryPoint(EntryPointType::Application, &create_alpha);
		server.addEntryPoint(EntryPointType::Application, &create_ide, "/ide");
		if (server.start()) {
			int sig = WServer::waitForShutdown();
			std::cerr << "Shutdown (signal = " << sig << ")" << std::endl;
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

alpha::alpha(const WEnvironment& env) : WApplication(env) {
	// addMetaHeader("viewport", "width=device-width, initial-scale=1");

	messageResourceBundle().use("messages/alpha", false);

	root()->setHeight("80%");

	WString title = tr("Tau Alpha");
	setTitle(title);
	root()->addWidget(
		std::make_unique<WText>(WString("<h1>{1}</h1>").arg(title)));

	create_views();

	useStyleSheet(resolveRelativeUrl("alpha.css"));
	useStyleSheet(resolveRelativeUrl("resources/bootstrap.reduced.css"));
}

void alpha::create_views() {
	root()->addWidget(make_unique<widget::agent_view>(
		agent{"007", "Bond", "James Bond"}));
	root()->addWidget(make_unique<widget::channel_view>(
		channel{ "0", "admin", "general", 0 }));
}

}
