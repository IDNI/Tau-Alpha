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
#ifndef __ALPHA_WIDGET_MESSAGE_VIEW_H__
#define __ALPHA_WIDGET_MESSAGE_VIEW_H__

#include <Wt/WApplication.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>

#include "../message.h"

namespace alpha {

namespace widget {

class message_view : public Wt::WTemplate {
	message m;
public:
	message_view(message m) : m(m) {
		Wt::WApplication *app = Wt::WApplication::instance();
		app->messageResourceBundle().use("messages/message");
		addStyleClass("message");
		render();

	}
	void render() {
		setTemplateText(tr("message"));
		bindWidget("author",  std::make_unique<Wt::WText>(m.author()));
		bindWidget("title",   std::make_unique<Wt::WText>(m.title()));
		bindWidget("content", std::make_unique<Wt::WText>(m.content()));
	}
};

}

}

#endif
