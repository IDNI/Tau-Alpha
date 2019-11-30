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
#include <sstream>

#include <Wt/WApplication.h>
#include <Wt/WText.h>

#include "agent.h"
#include "../app.h"

namespace alpha::wt::view {

using std::make_unique;

alpha::agent* agent::get_agent() {
	return a;
}

agent::agent(const render_type& type, alpha::agent* a) : type(type), a(a) {
	app::instance()->messageResourceBundle().use("messages/agent");
	std::stringstream ss; ss << "agent agent-" << type_names[type];
	addStyleClass(ss.str());
	if (!a) return;
	render();
}

void agent::set_agent(alpha::agent* newa) {
	a = newa;
	render();
}

void agent::render() {
	setTemplateText(tr(type_names[type]+"_agent_tpl"));
	bind_data();
}

void agent::bind_data() {
	if (!a) return;
	bindWidget("id",         std::make_unique<Wt::WText>(a->id));
	bindWidget("name",       std::make_unique<Wt::WText>(a->name));
	bindWidget("other_name", std::make_unique<Wt::WText>(a->other_name));
	bindWidget("created",    std::make_unique<Wt::WText>(
						print_date(a->created)));
}

}
