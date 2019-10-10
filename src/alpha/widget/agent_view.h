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
#ifndef __ALPHA_WIDGET_AGENT_VIEW_H__
#define __ALPHA_WIDGET_AGENT_VIEW_H__

#include <Wt/WApplication.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>

#include "../agent.h"

namespace alpha {

namespace widget {

class agent_view : public Wt::WTemplate {
	agent a;
public:
	agent_view(agent a) : a(a) {
		Wt::WApplication *app = Wt::WApplication::instance();
		app->messageResourceBundle().use("messages/agent");
		addStyleClass("agent");
		render();
	}
	void render() {
		setTemplateText(tr("agent"));
		bindWidget("id", std::make_unique<Wt::WText>(a.id()));
		bindWidget("name", std::make_unique<Wt::WText>(a.name()));
		bindWidget("other_name",
			std::make_unique<Wt::WText>(a.other_name()));
	}
};

}

}

#endif
