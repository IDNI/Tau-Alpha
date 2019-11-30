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

#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WText.h>

#include "message.h"
#include "agent.h"
#include "../app.h"
#include "../../protocol.h"

namespace alpha::wt::view {

using namespace Wt;
using std::make_shared;
using std::make_unique;
using agent_rt = agent::render_type;

message::message(const render_type& type, sp_message sm, on_send_fn_t cb)
: type(type), sm(sm), cb_(cb)
{
	if (!sm) sm = make_shared<alpha::message>();
	if (!sm) throw 0;
	app::instance()->messageResourceBundle().use("messages/message");
	std::stringstream ss; ss << "message message-" << type_names[type];
	addStyleClass(ss.str());
	render();
}

void message::render() {
	setTemplateText(tr(type_names[type]+"_msg_tpl"));
	if (type == FORM) render_form();
	else bind_data();
}

void message::render_form() {
	auto f = bindWidget("post_form", make_unique<WContainerWidget>());
	auto l = f->addWidget(make_unique<WLabel>("Subject:"));
	subject_ = f->addWidget(make_unique<WLineEdit>(sm?sm->subject:""));
	l->setBuddy(subject_);
	l = f->addWidget(make_unique<WLabel>("Content:"));
	content_ = f->addWidget(make_unique<WTextArea>(sm?sm->content:""));
	l->setBuddy(content_);
	send_ = f->addWidget(make_unique<WPushButton>(tr("SEND")));
	send_->clicked().connect(this, &message::send);
	if (cb_) send_->clicked().connect(cb_);
}

void message::bind_data() {
	if (!sm) return;
	auto a = app::instance()->agents()->get(sm->author).get();
	bindWidget("author",  make_unique<agent>(agent_rt::SHORT, a));
	bindWidget("subject", make_unique<WText>(sm->subject));
	bindWidget("content", make_unique<WText>(sm->content));
	std::stringstream ss; ss << sm->created;
	bindWidget("created", make_unique<WText>(ss.str()));
}

void message::send() {
	if (subject_->text().empty() || content_->text().empty()) return;
	if (!sm) sm = make_shared<alpha::message>();
	sm->subject = subject_->text().toUTF8();
	sm->content = content_->text().toUTF8();
	if (sm) protocol::send(app::instance()->get_session_id(), *(sm.get()));
	subject_->setText("");
	content_->setText("");
}

}
