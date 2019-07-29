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
	create_splitter();
	create_statusbar();

	WApplication::instance()->useStyleSheet(
		WApplication::instance()->resolveRelativeUrl("alpha.css"));

	std::stringstream ss;
	DBG(ss << tr(string("i_debug"));)
	NDBG(ss << tr(string("i_intro"));)
	editor_->setText(ss.str());
}

void alpha::create_splitter() {
	sc_ = c_->addWidget(make_unique<splitjs>());
	editor_ = sc_->first()->addWidget(make_unique<TML_editor>());
	output_ = sc_->second()->addWidget(make_unique<TML_editor>());

	editor_->setOption("lineNumbers", "'true'");
	editor_->onUpdate().connect([this](std::string prog){
		if (o.autorun) run_tml(prog), bdd::gc();
		else changed();
	});
	output_->setOption("readOnly", "'nocursor'");
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

	auto em = make_unique<WPopupMenu>(); // examples menu
	std::vector<string> intros = { "intro", "FACTS", "RELATIONS",
		"ARITY", "RULES", "VARIABLES", "AND/OR", "RECURSION",
		"TRANSITIVE CLOSURE", "DELETION", "NEGATION",
		"family", "armageddon", "UNSAT", "DYCK", "tml.g" };
	DBG(intros.push_back("debug");)
	for (string i : intros)
		em->addItem(tr(i))->triggered().connect([this, i]{
			std::stringstream ss; ss << tr(string("i_")+i);
			editor_->setText(ss.str());
		});
	auto eb = make_unique<WPushButton>(tr("examples"));
	eb->setMenu(move(em));
	tb->addButton(move(eb));
}

void alpha::create_statusbar() {
	sb_ = c_->addWidget(make_unique<WTemplate>(tr("statusbar")));
	update_status(status_);
	not_changed();
	elapsed(0);
}

void alpha::create_toolbar() {
	auto tb = c_->addWidget(make_unique<WToolBar>());
	auto run_btn = make_unique<WPushButton>(tr("RUN"));
	run_btn->clicked().connect([this]{ run_tml(); });
	run_btn_ = run_btn.get();
	tb->addButton(move(run_btn));
	auto runjs_btn = make_unique<WPushButton>(tr("RUN JS"));
	runjs_btn->clicked().connect([this]{ run_tml_js(); });
	runjs_btn_ = runjs_btn.get();
	tb->addButton(move(runjs_btn));
}

void alpha::log(std::string level, std::string message) {
	std::stringstream js; js
	<< "console.log(`" << level << "\n`, "
	<< std::quoted(message, '`') << ");";
	this->doJavaScript(js.str());
}

}
