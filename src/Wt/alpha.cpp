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
#include <iomanip>
#include <Wt/WEnvironment.h>
#include <Wt/WText.h>
#include "alpha.h"

Wt::WString tr(std::string s) { return Wt::WString::tr(s); }

std::map<status_values, std::string> status_name = {
	{ERROR, "error"}, {INIT, "init"}, {RUNNING, "running"},
	{STOPPED, "stopped"}, {FINISHED, "finished"}, {UNSAT, "unsat"},
	{CONTRADICTION, "contradiction"}
};

namespace Wt {

using std::make_unique;
using std::string;

int alpha::run(int argc, char** argv) {
	return WRun(argc, argv, [](const WEnvironment& env) {
		return make_unique<alpha>(env);
	});
}

alpha::alpha(const WEnvironment& env) :
	WApplication(env),
	result_(this, "result")
{
	// addMetaHeader("viewport", "width=device-width, initial-scale=1");

	// load messages from alpha.xml
	messageResourceBundle().use("messages/alpha", false);

	load_tml_js();

	// setup title, show title in header and add container c_ (div)
	WString title = tr("Tau Alpha");
	setTitle(title);
	root()->addWidget(
		std::make_unique<WText>(WString("<h1>{1}</h1>").arg(title)));
	c_ = root()->addWidget(make_unique<WContainerWidget>());

	create_menu();
	create_toolbar();
	create_splitters_ui();
	create_statusbar();

	editor_->setOption("lineNumbers", "'true'");
	editor_->onUpdate().connect([this](std::string prog){
		if (o.autorun) run_tml(prog), bdd::gc();
		else changed();
	});
	output_->setOption("readOnly", "'nocursor'");

	WApplication::instance()->useStyleSheet(
		WApplication::instance()->resolveRelativeUrl("alpha.css"));

	workspace_->setEditor(editor_);
	workspace_->setFile("./workspace", "01_intro.tml");
	workspace_->setTitle("intro");
	workspace_->onShowFile([this](){ update_status(INIT); });
}

void alpha::create_splitters_ui() {
	wsc_ = c_->addWidget(make_unique<splitjs>(splitjs::direction::HORIZONTAL, "30,70"));
	wsc_->addStyleClass("splitter_workspace");
	workspace_ = wsc_->first()->addWidget(make_unique<workspace>("./workspace/"));
	sc_ = wsc_->second()->addWidget(make_unique<splitjs>());
	sc_->addStyleClass("splitter_editor");
	editor_ = sc_->first()->addWidget(make_unique<TML_editor>());
	output_ = sc_->second()->addWidget(make_unique<TML_editor>());
}

void alpha::create_menu() {
	auto tb = c_->addWidget(make_unique<WToolBar>());
	auto sm = make_unique<WPopupMenu>();

	auto set_linenumbers = sm->addItem(tr("show line numbers"));
	set_linenumbers->setCheckable(true);
	set_linenumbers->setChecked(o.linenumbers);
	set_linenumbers->triggered().connect([set_linenumbers,this]{
		o.linenumbers = set_linenumbers->isChecked();
		editor_->setOption("lineNumbers", o.linenumbers?"true":"false");
	});

	auto set_autorun = sm->addItem(tr("autorun"));
	set_autorun->setCheckable(true);
	set_autorun->setChecked(o.autorun);
	set_autorun->triggered().connect([this,set_autorun]{
		o.autorun = set_autorun->isChecked();
	});

	auto sb = make_unique<WPushButton>(tr("settings"));
	sb->setMenu(move(sm));
	tb->addButton(move(sb));
}

void alpha::create_statusbar() {
	sb_ = c_->addWidget(make_unique<WTemplate>(tr("statusbar")));
	update_status(status_);
	not_changed();
	elapsed(0);
}

void alpha::create_toolbar() {
	auto tb = c_->addWidget(make_unique<WToolBar>());
#ifndef DISABLE_SERVER_EVALUATION
	auto run_btn = make_unique<WPushButton>(tr("RUN"));
	run_btn->clicked().connect([this]{ run_tml(); });
	run_btn_ = run_btn.get();
	tb->addButton(move(run_btn));
#endif
#ifndef DISABLE_LOCAL_EVALUATION
	auto runjs_btn = make_unique<WPushButton>(tr("RUN JS"));
	runjs_btn->clicked().connect([this]{ run_tml_js(); });
	runjs_btn_ = runjs_btn.get();
	tb->addButton(move(runjs_btn));
#endif
}

void alpha::log(std::string level, std::string message) {
	std::stringstream js; js
	<< "console.log(`" << level << "\n`, "
	<< std::quoted(message, '`') << ");";
	this->doJavaScript(js.str());
}

}
