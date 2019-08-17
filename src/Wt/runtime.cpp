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

void alpha::run_tml() {
	run_tml(editor_->getText());
}

unsigned long run_tml_counter = 0;
void alpha::run_tml(string prog) {
	unsigned long id = ++run_tml_counter;
	Wt::log("info")<<"TML(" << id << ") run";
	run_btn_->disable();
	update_status(RUNNING);
	not_changed();
	std::vector<string> args = {
		"--output", "@buffer",
		"--error",  "@buffer",
		"--info",   "@buffer",
		"--debug",  "@buffer"
	};
	output_->setText("");

	clock_t start = clock(), end;
	driver d(s2ws(prog), ::options(args));
	end = clock();
	bdd::gc();

	if (!d.result) update_status(UNSAT);
	else update_status(FINISHED);

	std::string s = ws2s(::output::read(s2ws(string("output"))));
	// trim new lines
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return ch != 10 && ch != 13;
	}));
	output_->setText(s);

	DBG(log("debug", ws2s(::output::read(s2ws(string("debug")))));)
	log("info", ws2s(::output::read(s2ws(string("info")))));
	log("error", ws2s(::output::read(s2ws(string("error")))));

	double e = 1000 * double(end - start) / CLOCKS_PER_SEC;
	Wt::log("info")<<"TML(" << id << ") " << status_name[status_]
	<< " - elapsed: " << e << " ms";
	elapsed(e);
	run_btn_->enable();
}

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
}

unsigned long run_tml_js_counter = 0;
void alpha::run_tml_js() {
	unsigned long id = ++run_tml_js_counter;
	Wt::log("info")<<"TML.js(" << id << ") run";
	update_status(RUNNING);
	not_changed();
	runjs_btn_->disable();
	output_->setText("");
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
			<< output_->jsValue() << " = output.read('output')"
			<< ".replace(/^\\n*/,'');\n"; // ltrim new lines
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
