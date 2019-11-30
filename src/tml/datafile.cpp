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
#include "relation.h"
#include "datafile.h"
#include "query.h"

namespace tml {

using std::string;
using std::wstring;
using std::make_shared;
using std::cout;
using std::endl;

void datafile::init() {
	bdd::init();
	driver::init();
}

strings datafile::cache_args() {
	strings ret{};
	ret = args;
	ret.push_back("--input"), ret.push_back(dfile);
	if (pfile != "") ret.push_back("--input"), ret.push_back(pfile);
	return ret;
}

datafile::datafile(const string& dfile, const string& pfile, strings args) :
	dfile(dfile), pfile(pfile), args(args), cached_args(cache_args()) {}
datafile::datafile(const string& dfile, strings args) :
	dfile(dfile), pfile(""), args(args), cached_args(cache_args()) {}

datafile datafile::load(const string& dfile, const string& pfile, strings args) {
	datafile df(dfile, pfile, args);
	df.load();
	return df;
}

datafile datafile::load(const string& dfile, strings args) {
	return datafile::load(dfile, string{}, args);
}

void datafile::load() {
	r = datafile::process(cached_args);
}

void datafile::save() {
	datafile::save(dfile, r);
}

void datafile::save(relations rels) {
	r = rels;
	save();
}

void datafile::save(const string& f, const relations& rels) {
	std::ofstream of(f);
	of << rels;
	of.close();
	// std::stringstream ss; ss << rels;
	// std::cout<<"saved into `"<<f<<"`:\n\t{\n"<<ss.str()<<"\n}\n"<<std::endl;
}

relations datafile::run(const string& e) {
	strings args = cached_args;
	args.push_back("--input-eval"), args.push_back(e);
	return process(args);
}

relations datafile::eval(const string& e) {
	return r = run(e);
}

relations datafile::update(const string& e) {
	eval(e);
	save();
	return r;
}

relations datafile::process(strings args, bool dontload) {

	//cout<<"args: "; for (auto& arg : args) cout<<arg<<" "; cout<<endl;

	args.push_back("--error"), args.push_back("@buffer");
	args.push_back("--no-debug");
	args.push_back("--no-info");
	args.push_back("--no-output");
	::options o(args);

	//cout<<"input: {\n"<<ws2s(o.input())<<"}\n"<<endl;

	driver d(o);

	string err = ws2s(output::read(L"error"));
	if (err != "") cout<<"datafile ERROR: `"<<err<<"`"<<endl;

	// rt_convertor to relations
	relations rels{};
	auto rt_convertor = [&rels](const raw_term& t) {
		wstring name = lexeme2str(t.e[0].e);
		auto it = rels.find(name);
		if (it == rels.end()) // new relation => add
			it = rels.emplace(name, make_shared<relation>(
				ws2s(name))).first;
		int row = it->second->rows();
		int col = 0;
		std::wstringstream es;
		for (size_t ar = 0, n = 1; ar != t.arity.size();) {
			es.str(L"");
			while (t.arity[ar] == -1) ++ar, es << L'(';
			if (n >= t.e.size()) break;
			while (t.e[n].type == elem::OPENP) ++n;
			for (int_t k = 0; k != t.arity[ar];)
				if ((es<<t.e[n++]), ++k != t.arity[ar]) {
					// set cell @ [row,col] in the relation
					it->second->set(row, col++,
								ws2s(es.str()));
					es.str(L"");
				}
			while (n<t.e.size() && t.e[n].type == elem::CLOSEP) ++n;
			++ar;
			while (ar < t.arity.size()
				&& t.arity[ar] == -2) ++ar, es<<L')';
			// set cell @ [row,col] in the relation
			if (es.str() != L"")
				it->second->set(row, col++, ws2s(es.str()));
		}
	};

	// decompress and convert every raw_term from the resulting database
	if (!dontload) d.out(rt_convertor);

	//cout<<"output: {\n"<<rels<<"\n}"<<endl;

	return rels;
}


void datafile::erase(const std::string& rname, int arity, std::string search,
	int col)
{
	update(query::erase(rname, arity, search, col));
}

bool datafile::exists(const std::string& rname, int arity, std::string search,
	int col)
{
	eval(query::exists(rname, arity, search, col));
	relation rel = get_relation(L"exists");
	return (rel.valid() && rel.rows() > 0);
}

row datafile::find(const std::string& rname, std::string search,
	int col)
{
	relation rel = get_relation(s2ws(rname));
	int row = rel.find_row(search, col);
	if (row == -1) return {};
	return rel.get_row(row);
}

int datafile::max(const std::string& rname, int col, int min) {
	relation rel = get_relation(s2ws(rname));
	int id{min};
	try {
		for (int i = 0; i != rel.rows(); ++i)
			id = std::max(id, std::stoi(rel.get(i, col)));
	} catch (...) { }
	return id;
}

void datafile::insert__(const string& name, size_t n, ...) {
	va_list args;
	va_start(args, n);
	update(query::insert_args__(name, n, args));
	va_end(args);
}

}
