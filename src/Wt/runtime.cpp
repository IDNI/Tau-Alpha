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
#include "alpha.h"

using namespace std;
using namespace Wt;

void alpha::before_run() {
	update_status(RUNNING);
	not_changed();
	output_->setText("");
	info_->clear();
	errors_->clear();
	//binary_->clear();
	DBG(debug_->clear();)
}

void alpha::run_tml() {
	run_tml(editor_->getText());
}

unsigned long run_tml_counter = 0;
void alpha::run_tml(string prog) {
	unsigned long id = ++run_tml_counter;
	Wt::log("info")<<"TML(" << id << ") run";
	run_btn_->disable();
	std::vector<string> args = {
		"--output",      "@buffer",
		"--error",       "@buffer",
		"--info",        "@buffer",
		"--debug",       "@buffer"
	};
	before_run();

	clock_t start = clock(), end;
	driver d(s2ws(prog), ::options(args));
	end = clock();

	while (tabular_tabs_->count() > 0) {
		tabular_tabs_->setCurrentIndex(0);
		tabular_tabs_->removeTab(tabular_tabs_->currentWidget());
	}
	tables_.clear();

	if (d.result) update_status(FINISHED), d.out([this](const raw_term& t) {
		wstring relation = lexeme2str(t.e[0].e);
		auto it = tables_.find(relation);
		WTable *table;
		if (it == tables_.end()) {
			auto new_table = make_unique<WTable>();
			table = new_table.get();
			table->setWidth("100%");
			table->addStyleClass("tabular-tabs");
			table->addStyleClass("table-hover");
			table->addStyleClass("table-condensed");
			table->addStyleClass("table-striped");
			tables_[relation] = table;
			auto item = tabular_tabs_->addTab(move(new_table), relation);
			item->addStyleClass("tabular-row");
		} else table = it->second;
		int row = table->rowCount();
		int col = 0;
		//Wt::log("info")<<"row: "<<row<<" relation: "<<ws2s(relation);
		//if (t.neg) table->elementAt(id, 0)->addNew<WText>("~");
		for (size_t ar = 0, n = 1; ar != t.arity.size();) {
			wstringstream es;
			while (t.arity[ar] == -1) ++ar, es << L'(';
			if (n >= t.e.size()) break;
			while (t.e[n].type == elem::OPENP) ++n;
			for (int_t k = 0; k != t.arity[ar];)
				if ((es<<t.e[n++]),++k!=t.arity[ar]) {
					//Wt::log("info")<<"col: "<<col<<" row: "<<row<<" relation: "<<ws2s(relation)<< " entity:'"<<ws2s(es.str())<<"'";
					table->elementAt(row, col++)
						->addNew<WText>(es.str());
					es.str(L"");
				}

			while (n<t.e.size() && t.e[n].type == elem::CLOSEP) ++n;
			++ar;
			while (ar < t.arity.size()
				&& t.arity[ar] == -2) ++ar, es<<L')';
			//if (ar > 1 && t.arity[ar-1] == -2 &&
			//	ar != t.arity.size()) es << L'';
			//Wt::log("info")<<"col: "<<col<<" row: "<<row<<" relation: "<<ws2s(relation)<< " entity:'"<<ws2s(es.str())<<"'";
			if (es.str() != L"")
				table->elementAt(row, col++)
					->addNew<WText>(es.str());
		}
	});
	else update_status(UNSAT);

	//std::string bin = serialize_result(d);

	bdd::gc();

	string s = ws2s(::output::read(L"output"));
	// trim new lines
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return ch != 10 && ch != 13;
	}));

	// log("bin", bin);
	// char h[100];
	// stringstream bs;
	// long n = 1;
	// for (unsigned char b : bin) {
	// 	sprintf(h, "%02x", b);
	// 	bs << h << " ";
	// 	if (n > 0 && !(n % 4) && n % 32) bs << " ";
	// 	if (n > 0 && !(n % 32) ) bs << "\n";
	// 	++n;
	// }
	// log("binary", bs.str());
	// add_text(binary_, bs.str());

	DBG(log("debug", ws2s(::output::read(L"debug")));)
	log("info", ws2s(::output::read(L"info")));
	log("error", ws2s(::output::read(L"error")));

	output_->setText(s);
	DBG(add_text(debug_, ::output::read(L"debug"));)
	add_text(info_, ::output::read(L"info"));
	add_text(errors_, ::output::read(L"error"));

	double e = 1000 * double(end - start) / CLOCKS_PER_SEC;
	Wt::log("info")<<"TML(" << id << ") " << status_name[status_]
	<< " - elapsed: " << e << " ms";
	elapsed(e);
	run_btn_->enable();
}

/*
std::string alpha::serialize_result(driver &d) {
	std::stringstream bs;
	//log("serialize_result");
	d.to_bin(bs);
	//log("serialized: ", bs.str());

	return bs.str();
}

void alpha::unserialize_result(driver &d, std::string bin) {
	std::istringstream is(bin);
	d.from_bin(is);
}
*/

void alpha::load_tml_js() {
	WApplication* app = WApplication::instance();
	app->require(app->resolveRelativeUrl("tml.js"));
	std::stringstream js;
	js
	<< "const { bdd, driver } = tml;\n"
	<< "bdd.init();\n"
	<< "driver.init();\n";
	Wt::log("info") << js.str();
	doJavaScript(js.str());
	Wt::log("info") << "loaded";
}

unsigned long run_tml_js_counter = 0;
void alpha::run_tml_js() {
	unsigned long id = ++run_tml_js_counter;
	Wt::log("info")<<"TML.js(" << id << ") run";
	runjs_btn_->disable();
	before_run();
	if (!result_.isConnected())
		result_.connect([this, id](bool r, double e){ // result+elapsed
			update_status(r ? FINISHED : UNSAT);
			Wt::log("info")<<"TML.js(" << id << ") "
			<< status_name[status_]
			<< " - elapsed: " << e << " ms";
			elapsed(e);

			std::stringstream js;
			js
			<< "var { output } = tml;\n"
			<< "var err = output.read('error');"
			<< output_->jsValue() << " = output.read('output')"
			<< ".replace(/^\\n*/,'')" // ltrim new lines
			<< " + (err != '' ? err.replace(/^/mg,'# ') : '');";
			DBG(js<<"console.log('debug', output.read('debug'));";)
			js
			<< "console.log('info', output.read('info'));"
			<< "console.log('error', output.read('error'));";
			Wt::log("info") << js.str();
			this->doJavaScript(js.str());
			output_->sync();
			runjs_btn_->enable();
		});
	std::stringstream js;
	js
	<< "const { bdd, driver, options, output, strings } = tml;\n"
	<< "const as = new strings();\n" // arguments
	<< "as.push_back('--output');\n"
	<< "as.push_back('@buffer');\n"
	<< "as.push_back('--error');\n"
	<< "as.push_back('@buffer');\n"
	<< "as.push_back('--info');\n"
	<< "as.push_back('@buffer');\n"
	<< "as.push_back('--debug');\n"
	<< "as.push_back('@buffer');\n"
	<< "const o = new options();\n"
	<< "o.parse(as, false);\n"
	<< "const start = performance.now();\n"
	<< "const d = driver.create(" << editor_->jsValue() << ", o);\n"
	<< "const e = performance.now() - start;\n"
	<< result_.createCall({"d.result", "e"}) << "\n"
	<< "d.delete(); o.delete(); as.delete();\n"
	<< "bdd.gc();\n";
	Wt::log("info") << js.str();
	this->doJavaScript(js.str());
}
