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
#include "ide.h"
#include "../app.h"

namespace alpha::wt::ide {

using namespace Wt;
using std::make_unique;
using std::string;

std::map<status_values, string> ide::status_name = {
	{ERROR, "error"}, {INIT, "init"}, {RUNNING, "running"},
	{STOPPED, "stopped"}, {FINISHED, "finished"}, {UNSAT, "unsat"},
	{CONTRADICTION, "contradiction"}
};

ide::ide() :
	result_(this, "result"),
	relation_ensure_(this, "relation_ensure"),
	relation_set_(this, "relation_out_set"),
	output_finished_(this, "output_finished"),
	set_tab_text_(this, "set_tab_text")
{
	app *a = (app *) Wt::WApplication::instance();
	a->messageResourceBundle().use("messages/ide", false);
	//a->set_title(tr("Tau Alpha")+" - "+tr("TML IDE"));
	addStyleClass("ide");

	render();

#ifndef DISABLE_FRONTEND_EXECUTION
	runtime_frontend_load();
#endif

	editor_->setOption("lineNumbers", "'true'");
	editor_->onUpdate().connect([this](std::string){
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

	workspace_->setEditor(editor_);
	workspace_->setFile("./workspace", "01_intro.tml");
	workspace_->setTitle("intro");
	workspace_->onShowFile([this](){ update_status(INIT); });
}

void ide::render() {
	setTemplateText(tr("ide_template"));

	menu_ = bindWidget("menu", std::make_unique<menu>(
		o.linenumbers,
		[this](Wt::WMenuItem *mi) -> void {
			o.linenumbers = mi->isChecked();
			editor_->setOption("lineNumbers", o.linenumbers
				? "true" : "false");
		},
		o.autorun,
		[this](Wt::WMenuItem *mi) -> void {
			o.autorun = mi->isChecked();
		}
	));

	toolbar_ = bindWidget("toolbar", std::make_unique<toolbar>(
		[this](){ runtime_backend(); },
		[this](){ runtime_frontend(); }
	));

	ui_ = bindWidget("ui", std::make_unique<splitjs>(
				splitjs::direction::HORIZONTAL, "20,80"));
	ui_->addStyleClass("splitter_workspace");

	workspace_ = ui_->first()->addWidget(make_unique<workspace>(
 								"./workspace"));

 	iui_ = ui_->second()->addWidget(make_unique<splitjs>());
 	iui_->addStyleClass("splitter_editor");

 	editor_ = iui_->first()->addWidget(make_unique<TML_editor>());
// 	auto c = sc_->second()->addWidget(make_unique<WContainerWidget>());
// 	c->resize("100%", "100%");

	tabs_ = iui_->second()->addWidget(make_unique<WTabWidget>());
 	tabs_->setStyleClass("tabwidget");
 	//tabs_->resize("100%", "100%");

 	// output tab
 	auto output = make_unique<TML_editor>();
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

	statusbar_ = bindWidget("statusbar", std::make_unique<WTemplate>(
						tr("statusbar_template")));
	update_status(status_);
	not_changed();
	elapsed(0);
}

void ide::refresh_tabs() {
	tabs_->setCurrentIndex(tabs_->currentIndex());
	tabular_tabs_->setCurrentIndex(tabular_tabs_->currentIndex());
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
