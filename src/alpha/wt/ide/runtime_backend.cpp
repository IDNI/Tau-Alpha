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

void ide::runtime_backend(string prog) {
#ifdef DISABLE_BACKEND_EXECUTION
	return;
#endif
	static unsigned long id{0};
	Wt::log("info")<<"TML(" << ++id << ") backend run";
	toolbar_->run_backend_btn_->disable();
	std::vector<string> args = {
		"--output",      "@buffer",
		"--error",       "@buffer",
		"--info",        "@buffer",
		"--debug",       "@buffer"
	};
	runtime_before();

	clock_t start = clock(), end;
	driver d(s2ws(prog), ::options(args));
	end = clock();

	if (d.result) update_status(FINISHED), d.out([this](const raw_term& t) {
		WTable *table = get_table(lexeme2str(t.e[0].e));
		int row = table->rowCount();
		int col = 0;
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
			if (es.str() != L"")
				table->elementAt(row, col++)
					->addNew<WText>(es.str());
		}
	});
	else update_status(UNSAT);

	std::string bin = serialize_result(d);

	bdd::gc();

	string s = ws2s(::output::read(L"output"));
	// trim new lines
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return ch != 10 && ch != 13;
	}));

	string hex = bin2hex(bin);
	//DBG(log("binary", hex));
	add_text(binary_, hex);

	//DBG(log("debug", ws2s(::output::read(L"debug")));)
	//log("info", ws2s(::output::read(L"info")));
	//log("error", ws2s(::output::read(L"error")));

	output_->setText(s);
	DBG(add_text(debug_, ::output::read(L"debug"));)
	add_text(info_, ::output::read(L"info"));
	add_text(errors_, ::output::read(L"error"));

	double e = 1000 * double(end - start) / CLOCKS_PER_SEC;
	Wt::log("info")<<"TML(" << id << ") " << ide::status_name[status_]
	<< " - elapsed: " << e << " ms";
	elapsed(e);

	toolbar_->run_backend_btn_->enable();

	//DBG(unserialize_result(d, bin);)

	runtime_after();
}

std::string ide::serialize_result(driver &d) {
	std::stringstream bs; bs << d;
	return bs.str();
}

void ide::unserialize_result(driver &d, std::string bin) {
	std::istringstream is(bin); is >> d;
}

}
