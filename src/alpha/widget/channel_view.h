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
#ifndef __ALPHA_WIDGET_CHANNEL_VIEW_H__
#define __ALPHA_WIDGET_CHANNEL_VIEW_H__

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <Wt/WBreak.h>

#include "../channel.h"
#include "message_view.h"

namespace alpha {

namespace widget {

class channel_view : public Wt::WTemplate {
	channel ch;
public:
	channel_view(channel ch) : ch(ch) {
		Wt::WApplication *app = Wt::WApplication::instance();
		app->messageResourceBundle().use("messages/channel");
		addStyleClass("channel");
		render();
	}
	void render() {
		setTemplateText(tr("channel"));
		bindWidget("op", std::make_unique<Wt::WText>(ch.op()));
		bindWidget("title", std::make_unique<Wt::WText>(ch.title()));
		auto pmsgs = std::make_unique<Wt::WContainerWidget>();
		auto msgs = pmsgs.get();
		bindWidget("messages", std::move(pmsgs));
		for (auto m : ch.messages()) {
			msgs->addWidget(std::make_unique<message_view>(m));
			msgs->addWidget(std::make_unique<Wt::WBreak>());
		}
		if (!ch.messages().size())
			msgs->addWidget(std::make_unique<Wt::WText>(tr("no msgs")));
	}
};

}

}

#endif
