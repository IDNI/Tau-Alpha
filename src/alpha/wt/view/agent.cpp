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

sp_agent agent::get_agent(const agent_id& aid) {
	return app::instance()->agents()->get(aid);
}

agent::agent(const render_type& type, const agent_id& aid)
 : agent(type, get_agent(aid)) {}

agent::agent(const render_type& type, sp_agent sa) : type(type), sa(sa) {
	app::instance()->messageResourceBundle().use("messages/agent");
	std::stringstream ss; ss << "agent agent-" << type_names[type];
	addStyleClass(ss.str());
	if (!sa) return;
	render();
}

void agent::render() {
	setTemplateText(tr(type_names[type]+"_agent_tpl"));
	bind_data();
}

void agent::bind_data() {
	if (!sa) return;
	bindWidget("id",         std::make_unique<Wt::WText>(sa->id));
	bindWidget("name",       std::make_unique<Wt::WText>(sa->name));
	bindWidget("other_name", std::make_unique<Wt::WText>(sa->other_name));
}



}
