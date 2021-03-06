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
#include <Wt/WContainerWidget.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <Wt/WBreak.h>

#include "agent.h"
#include "message.h"
#include "channel.h"
#include "../app.h"

namespace alpha::wt::view {

using namespace Wt;
using std::make_unique;
using std::make_shared;
using std::move;
using agent_rt = view::agent::render_type;
using message_rt = view::message::render_type;
using alpha::sp_message;

channel::channel(alpha::channel* ch, bool form) : ch(ch), form(form) {
	app::instance()->messageResourceBundle().use("messages/channel");
	addStyleClass("channel");
	render();
}

void channel::render() {
	setTemplateText(tr("channel_tpl"));
	bind_data();
}

void channel::bind_data() {
	if (!ch) return;
	auto c = app::instance()->agents()->get(ch->creator).get();
	bindWidget("creator", make_unique<agent>(agent_rt::SHORT, c));
	bindWidget("name", make_unique<WText>(ch->name));
	std::stringstream ss; ss << ch->created;
	bindWidget("created", make_unique<WText>(ss.str()));

	bind_messages();
	if (form) {
		sp_message post = make_shared<alpha::message>();
		post->targets.push_back(ch->id);
		post_form = bindWidget("post_form", make_unique<message>(
			message_rt::FORM, post,	[this](){ bind_messages(); }
		));
	} else {
		bindWidget("post_form", make_unique<WText>(""));
		post_form = 0;
	}
}

struct created_before {
	inline bool operator() (const sp_message& m1, const sp_message& m2) {
		return (m1->created < m2->created);
	}
};

void channel::bind_messages() {
	auto pmsgs = make_unique<WContainerWidget>();
	auto msgs = pmsgs.get();
	bindWidget("messages", move(pmsgs));
	message_f f{}; f.targets = { ch->id };
	auto ms = app::instance()->messages()->get_list(f);
	if (!ms.size())	msgs->addWidget(make_unique<WText>(tr("no msgs")));
	else {
		sort(ms.begin(), ms.end(), created_before());
		for (sp_message &m : ms) {
			msgs->addWidget(make_unique<message>(
							message_rt::DETAIL, m));
			msgs->addWidget(make_unique<WBreak>());
		}
	}
}

}
