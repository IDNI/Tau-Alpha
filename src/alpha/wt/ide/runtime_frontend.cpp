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

namespace alpha::wt::ide {

using namespace std;
using namespace Wt;

void ide::runtime_frontend_load() {
#ifdef DISABLE_FRONTEND_EXECUTION
	return;
#endif
	WApplication* app = WApplication::instance();
	app->require(app->resolveRelativeUrl("tml.js"));
	stringstream js;
	js << R"(
const { bdd, driver } = tml;
bdd.init();
driver.init();
)";
	Wt::log("info") << js.str();
	doJavaScript(js.str());
	Wt::log("info") << "loaded";
}

void ide::runtime_frontend() {
#ifdef DISABLE_FRONTEND_EXECUTION
	return;
#endif
	static unsigned long stid{0}; unsigned long id = ++stid;
	Wt::log("info")<<"TML.js(" << id << ") frontend run";
	toolbar_->run_frontend_btn_->disable();
	runtime_before();
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
			toolbar_->run_frontend_btn_->enable();

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
			<< set_tab_text_.createCall({"'info'",
						"output.read('info')"})<<"\n"
			<< set_tab_text_.createCall({"'error'",
						"output.read('error')"})<<"\n";
			DBG(js<<set_tab_text_.createCall({"'debug'",
						"output.read('debug')"})<<"\n";)
			DBG(js<<"console.log('debug', output.read('debug'));";)
			js
			<< "console.log('info', output.read('info'));"
			<< "console.log('error', output.read('error'));";

			Wt::log("info") << js.str();
			this->doJavaScript(js.str());
			output_->sync();
		});
	if (!relation_ensure_.isConnected()) relation_ensure_.connect(
		[this](string r) { get_table(s2ws(r)); });
	if (!relation_set_.isConnected()) relation_set_.connect(
		[this](string r, int row, string val) {
			static string l_r = r;
			static int l_row = row;
			static int x = 0;
			static int y = 0;
			if (l_r != r) l_r = r, l_row = y = x = 0;
			else if (l_row != row) l_row = row, ++y, x = 0;
			WTable *table = get_table(s2ws(r));
			// Wt::log("info") << "r: `" << r
			// << "` row: `" << row << "` val: `" << val << "`"
			// << "` y: `" << y << "` x: `" << x;
			table->elementAt(y, x++)->addNew<WText>(val);
		});
	if (!output_finished_.isConnected()) output_finished_.connect([this]() {
		runtime_after();
	});
	stringstream js;
	js << R"(
const { bdd, driver, options, output, strings } = tml;
const as = new strings(); // arguments
as.push_back('--output');
as.push_back('@buffer');
as.push_back('--error');
as.push_back('@buffer');
as.push_back('--info');
as.push_back('@buffer');
as.push_back('--debug');
as.push_back('@buffer');
const o = new options();
o.parse(as, false);
const start = performance.now();
const d = driver.create()"<<editor_->jsValue()<<R"(, o);
const e = performance.now() - start;
const uint8array = d.to_bin();
let n = 1;
const bin2hex = buffer => {
	const byteArray = new Uint8Array(buffer);
	return Array.from(byteArray, function(byte) {
		var hex = Number(byte).toString(16);
		if (hex.length < 2) hex = '0' + hex;
		hex += ' ';
		if (!(n % 4) && n % 32) hex += ' ';
		if (!(n % 32)) hex += '\n';
		++n;
		return hex;
	}).join('');
};
const bin = bin2hex(uint8array);
class tabular_collector_)"<<id<<R"( {
	constructor() {
	//console.log('tabular_collector_)"<<id<<R"( created');
		this.rels = {};
	}
	length(r) {
//		console.log('length(', r, ')');
		)"<<relation_ensure_.createCall({"r"})<<R"(
		this.rels[r] = this.rels[r] || 0;
		return this.rels[r];
	}
	set(r, row, col, val) {
		//console.log('set('+r+', '+row+', '+col+', '+val+')');
		)"<<relation_set_.createCall({"r", "row", "val"})<<R"(
		this.rels[r]++;
	}
}
if (d.result) )"<<result_.createCall({"d.result", "e", "bin"})<<R"(;
setTimeout(function () {
	if (d.result) d.out(new tabular_collector_)"<<id<<R"(());
	d.delete(); o.delete(); as.delete();
	bdd.gc();
	)"<<output_finished_.createCall({})<<R"(
}, 0);
)";
	Wt::log("info") << js.str();
	doJavaScript(js.str());
}

}
