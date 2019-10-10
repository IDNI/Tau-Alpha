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
#include <Wt/WTabWidget.h>
#include <Wt/WEnvironment.h>
#include <Wt/WText.h>
#include <Wt/WStackedWidget.h>
#include "ide.h"

namespace alpha {

using namespace Wt;
using std::make_unique;
using std::string;

std::map<status_values, string> ide::status_name = {
	{ERROR, "error"}, {INIT, "init"}, {RUNNING, "running"},
	{STOPPED, "stopped"}, {FINISHED, "finished"}, {UNSAT, "unsat"},
	{CONTRADICTION, "contradiction"}
};

int ide::start(int argc, char** argv) {
	return WRun(argc, argv, [](const WEnvironment& env) {
		return make_unique<ide>(env);
	});
}

ide::ide(const WEnvironment& env) :
	WApplication(env),
	result_(this, "result"),
	relation_ensure_(this, "relation_ensure"),
	relation_set_(this, "relation_out_set"),
	output_finished_(this, "output_finished"),
	set_tab_text_(this, "set_tab_text")
{
	// addMetaHeader("viewport", "width=device-width, initial-scale=1");

	// load messages from alpha.xml
	messageResourceBundle().use("messages/ide", false);

	runtime_frontend_load();

	// setup title, show title in header and add container c_ (div)
	WString title = tr("TML IDE");
	setTitle(title);
	root()->addWidget(
		std::make_unique<WText>(WString("<h1>{1}</h1>").arg(title)));

	c_ = root()->addWidget(make_unique<WContainerWidget>());
	c_->setHeight("80%");

	create_menu();
	create_toolbar();
	create_splitters_ui();
	create_statusbar();

	editor_->setOption("lineNumbers", "'true'");
	editor_->onUpdate().connect([this](std::string prog){
		if (o.autorun) {
#ifndef DISABLE_FRONTEND_EXECUTION
			runtime_frontend();
#else
#	ifndef DISABLE_BACKEND_EXECUTION
			runtime_backend();
			bdd::gc();
#	endif
#endif
		} else changed();
	});
	output_->setOption("readOnly", "'nocursor'");

	useStyleSheet(resolveRelativeUrl("alpha.css"));
	useStyleSheet(resolveRelativeUrl("resources/bootstrap.reduced.css"));
	workspace_->setEditor(editor_);
	workspace_->setFile("./workspace", "01_intro.tml");
	workspace_->setTitle("intro");
	workspace_->onShowFile([this](){ update_status(INIT); });
}

void ide::create_splitters_ui() {
	wsc_ = c_->addWidget(make_unique<widget::splitjs>(
			widget::splitjs::direction::HORIZONTAL, "20,80"));
	wsc_->addStyleClass("splitter_workspace");
	workspace_ = wsc_->first()->addWidget(make_unique<widget::workspace>(
		"./workspace"));
	sc_ = wsc_->second()->addWidget(make_unique<widget::splitjs>());
	sc_->addStyleClass("splitter_editor");

	editor_ = sc_->first()->addWidget(make_unique<widget::TML_editor>());
	auto c = sc_->second()->addWidget(make_unique<WContainerWidget>());
	c->resize("100%", "100%");

	tabs_ = c->addNew<WTabWidget>();
	tabs_->setStyleClass("tabwidget");
	//tabs_->resize(WLength(100, LengthUnit::Percentage), WLength(100, LengthUnit::Percentage));

	// output tab
	auto output = make_unique<widget::TML_editor>();
	output_ = output.get();
	tabs_->addTab(move(output), "output");

	// tabular tab
	auto tabular = make_unique<WContainerWidget>();
	tabular_ = tabular.get();
	tabular_->resize("100%", "100%");
	tabs_->addTab(move(tabular), "tabular");

	// tabular tab widget containing out tables
	tabular_tabs_ = tabular_->addWidget(make_unique<WTabWidget>());
	tabular_tabs_->setStyleClass("tabwidget tabular-tabs");

	// WAnimation animation(AnimationEffect::SlideInFromLeft, TimingFunction::EaseIn);
	// tabular_tabs_->contentsStack()->setTransitionAnimation(animation, true);

	// binary tab
	auto binary = make_unique<WContainerWidget>();
	binary_ = binary.get();
	binary_->addStyleClass("text-tab binary-tab");
	tabs_->addTab(move(binary), "bin");

	// info tab
	auto info = make_unique<WContainerWidget>();
	info_ = info.get();
	info_->addStyleClass("text-tab info-tab");
	tabs_->addTab(move(info), "info");

	// errors tab
	auto errors = make_unique<WContainerWidget>();
	errors_ = errors.get();
	errors_->addStyleClass("text-tab error-tab");
	tabs_->addTab(move(errors), "errors");

	// debug tab
	auto debug = make_unique<WContainerWidget>();
	debug_ = debug.get();
	debug_->addStyleClass("text-tab debug-tab");
#ifdef DEBUG
	tabs_->addTab(move(debug), "debug");
#endif
}

void ide::create_menu() {
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

void ide::create_statusbar() {
	sb_ = c_->addWidget(make_unique<WTemplate>(tr("statusbar")));
	update_status(status_);
	not_changed();
	elapsed(0);
}

void ide::refresh_tabs() {
	tabs_->setCurrentIndex(tabs_->currentIndex());
	tabular_tabs_->setCurrentIndex(tabular_tabs_->currentIndex());
}

void ide::create_toolbar() {
	auto tb = c_->addWidget(make_unique<WToolBar>());
#ifndef DISABLE_BACKEND_EXECUTION
	auto bbtn = make_unique<WPushButton>(tr("RUN"));
	bbtn->clicked().connect([this]{ runtime_backend(); });
	run_backend_btn_ = bbtn.get();
	tb->addButton(move(bbtn));
#endif
#ifndef DISABLE_FRONTEND_EXECUTION
	auto fbtn = make_unique<WPushButton>(tr("RUN JS"));
	fbtn->clicked().connect([this]{ runtime_frontend(); });
	run_frontend_btn_ = fbtn.get();
	tb->addButton(move(fbtn));
#endif
}

void ide::log(std::string level, std::string message) {
	std::stringstream js; js
	<< "console.log(`" << level << "\n`, "
	<< std::quoted(message, '`') << ");";
	this->doJavaScript(js.str());
}

void ide::add_text(WContainerWidget* w, const std::string& text) {
	w->addWidget(std::make_unique<WText>(WString(text)));
}

void ide::add_text(WContainerWidget* w, const std::wstring& text) {
	add_text(w, ws2s(text));
}

WTable* ide::get_table(std::wstring r) {
	auto it = tables_.find(r);
	WTable *table;
	if (it == tables_.end()) {
		auto new_table = make_unique<WTable>();
		table = new_table.get();
		table->setWidth("100%");
		table->addStyleClass("tabular-tabs");
		table->addStyleClass("table-hover");
		table->addStyleClass("table-condensed");
		table->addStyleClass("table-striped");
		tables_[r] = table;
		auto item = tabular_tabs_->addTab(move(new_table), r);
		item->addStyleClass("tabular-row");
		return table;
	}
	return it->second;
}

void ide::runtime_clear() {
	output_->setText("");
	info_->clear();
	errors_->clear();
	binary_->clear();
	DBG(debug_->clear();)
	while (tabular_tabs_->count() > 0) {
		tabular_tabs_->setCurrentIndex(0);
		tabular_tabs_->removeTab(tabular_tabs_->currentWidget());
	}
	tables_.clear();
}

std::string bin2hex(const std::string& bin) {
	char h[100];
	std::stringstream bs;
	long n = 1;
	for (unsigned char b : bin) {
		sprintf(h, "%02x", b);
		bs << h << " ";
		if (!(n % 4) && n % 32) bs << " ";
		if (!(n % 32) ) bs << "\n";
		++n;
	}
	return bs.str();
}

}
