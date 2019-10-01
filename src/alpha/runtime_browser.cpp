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

namespace alpha {

using namespace std;
using namespace Wt;

void alpha::load_tml_js() {
	WApplication* app = WApplication::instance();
	app->require(app->resolveRelativeUrl("tml.js"));
	stringstream js;
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
	if (!set_tab_text_.isConnected()) set_tab_text_.connect(
		[this](std::string tab, std::string text) {
			WContainerWidget *t = 0;
			if (tab == "debug") t = debug_;
			else if (tab == "info") t = info_;
			else if (tab == "error") t = errors_;
			else if (tab == "binary") t=binary_, text=bin2hex(text);
			add_text(t, text);
		});
	if (!result_.isConnected())
		result_.connect([this, &id](bool r, double e, std::string bin) {
			update_status(r ? FINISHED : UNSAT);
			Wt::log("info")<<"TML.js(" << id << ") "
			<< status_name[status_]
			<< " - elapsed: " << e << " ms";
			elapsed(e);

			DBG(Wt::log("binary") << bin);
			add_text(binary_, bin);

			stringstream js;
			js
			<< "var { output } = tml;\n"
			<< output_->jsValue() << " = output.read('output')"
			<< ".replace(/^\\n*/,'');\n" // ltrim new lines
			<< set_tab_text_.createCall({"'info'", "output.read('info')"}) << "\n"
			<< set_tab_text_.createCall({"'error'", "output.read('error')"}) << "\n";
			DBG(js<<set_tab_text_.createCall({"'debug'", "output.read('debug')"}) << "\n";)
			DBG(js<<"console.log('debug', output.read('debug'));";)
			js
			<< "console.log('info', output.read('info'));"
			<< "console.log('error', output.read('error'));";

			Wt::log("info") << js.str();
			this->doJavaScript(js.str());
			output_->sync();
			runjs_btn_->enable();
		});
	if (!relation_ensure_.isConnected()) relation_ensure_.connect(
		[this](string r) { get_table(s2ws(r)); });
	if (!relation_set_.isConnected()) relation_set_.connect(
		[this](string r, int row, string val) {
			//Wt::log("info") << "r: `" << r << "` row: `"<<row<<"` val: `"<<val<<"`";
			static string l_r = r;
			static int l_row = row;
			static int col = 0;
			if (l_r != r) l_r = r, l_row = 0, col = 0;
			else if (l_row != row) l_row = row, col = 0;
			WTable *table = get_table(s2ws(r));
			table->elementAt(row, col++)->addNew<WText>(val);
		});
	if (!output_finished_.isConnected()) output_finished_.connect([this]() {
		refresh_tabs();
	});
	stringstream js;
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
	<< "const uint8array = d.to_bin();\n"
	<< "let n = 1;"
	<< "const bin2hex = buffer => {\n"
	<< "	const byteArray = new Uint8Array(buffer);\n"
	<< "	return Array.from(byteArray, function(byte) {\n"
	<< "		var hex = Number(byte).toString(16);\n"
	<< "		if (hex.length < 2) hex = '0' + hex;\n"
	<< "		hex += ' ';\n"
	<< "		if (!(n % 4) && n % 32) hex += ' ';\n"
	<< "		if (!(n % 32)) hex += \"\\n\";\n"
	<< "		++n;\n"
	<< "		return hex;\n"
	<< "	}).join('');\n"
	<< "};\n"
	<< "const bin = bin2hex(uint8array);\n"
	<< result_.createCall({"d.result", "e", "bin"}) << "\n"
	<< "class tabular_collector_" << id << " {" << "\n"
	<< "	constructor() { this.rels = {}; }" << "\n"
	<< "	length(r) {" << "\n"
	<< "		" << relation_ensure_.createCall({"r"}) << "\n"
	<< "		this.rels[r] = this.rels[r] || 0;" << "\n"
	<< "		return this.rels[r];" << "\n"
	<< "	}" << "\n"
	<< "	set(r, row, col, val) {" << "\n"
	<< "		" << relation_set_
				.createCall({"r", "row", "val"}) << "\n"
	<< "		this.rels[r]++;\n"
	<< "	}" << "\n"
	<< "}" << "\n"
	<< "if (d.result) {" << "\n"
	<< "	const tc = new tabular_collector_" << id << "();" << "\n"
	<< "	d.out(tc); tc.rels = {};" << "\n"
	<< "}" << "\n"
	<< output_finished_.createCall({}) << "\n"
	<< "d.delete(); o.delete(); as.delete();\n"
	<< "bdd.gc();\n";
	Wt::log("info") << js.str();
	this->doJavaScript(js.str());
}

}
