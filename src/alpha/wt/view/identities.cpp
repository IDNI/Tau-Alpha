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
#include <Wt/WBreak.h>
#include <Wt/WImage.h>
#include <Wt/WLabel.h>
#include <Wt/WPushButton.h>
#include <Wt/WTableCell.h>
#include <Wt/WTextArea.h>
#include <Wt/WText.h>
#include <Wt/WRegExpValidator.h>
#include <Wt/WSelectionBox.h>

#include <Wt/WCssDecorationStyle.h>
#include <Wt/WColor.h>

#include "identities.h"
#include "../../protocol.h"

namespace alpha::wt::view {

using namespace Wt;
using std::make_unique;

identities::identities(tml::relationfile db, on_new_session_id e)
	: WTable(), db(db), on_new_sid(e)
{
	create_ui();
}

void identities::create_ui() {
	WLabel *label;
	int row = 0;

	elementAt(row, 0)->setColumnSpan(3);
	elementAt(row, 0)->setContentAlignment(
		AlignmentFlag::Top | AlignmentFlag::Center);
	elementAt(row, 0)->setPadding(10);
	WText *title = elementAt(row,0)->addWidget(
		make_unique<WText>(tr("select identity")));
	title->decorationStyle().font().setSize(FontSize::XLarge);

	// error messages
	++row;
	elementAt(row, 0)->setColumnSpan(3);
	error_messages_ = elementAt(row, 0);
	error_messages_->setPadding(5);

	WCssDecorationStyle& error_style = error_messages_->decorationStyle();
	error_style.setForegroundColor(WColor("red"));
	error_style.font().setSize(FontSize::Smaller);
	error_style.font().setWeight(FontWeight::Bold);
	error_style.font().setStyle(FontStyle::Italic);

	// identity
	++row;
	identity_ = elementAt(row,2)->addWidget(make_unique<WSelectionBox>());
	db.load();
	tml::relation rel = db.get_relation();
	const std::vector<tml::row> &t = rel.get_table();
	for (int i = 0; i != (int)t.size(); ++i) {
		idmap.push_back(t[i][0]);
		std::stringstream ss;
		ss << t[i][1] << (t[i][2] == "" ? "" : " / ") << t[i][2];
		identity_->addItem(ss.str());
	}
	label = elementAt(row,0)->addWidget(
		make_unique<WLabel>(tr("label.identity")));
	label->setBuddy(identity_);
	identity_->activated().connect(this, &identities::submit);
	identity_->setFocus(true);

	// info messages
	++row;
	elementAt(row, 0)->setColumnSpan(3);
	info_messages_ = elementAt(row, 0);
	info_messages_->setPadding(5);

	WCssDecorationStyle& info_style = info_messages_->decorationStyle();
	// info_style.setForegroundColor(WColor("red"));
	info_style.font().setSize(FontSize::Smaller);
	info_style.font().setWeight(FontWeight::Bold);
	info_style.font().setStyle(FontStyle::Italic);

	// Set column widths for label and validation icon
	elementAt(2, 0)->resize(WLength(30, LengthUnit::FontEx), WLength::Auto);
	elementAt(2, 1)->resize(20, WLength::Auto);
}

void identities::set_agent(const agent_id& aid) {
	session_id sid = protocol::login(aid);
	if (sid == "") {
		error_messages_->addWidget(make_unique<WText>(
			"login failed"));
		error_messages_->addWidget(make_unique<WBreak>());
	} else {
		for (size_t i = 0; i != idmap.size(); ++i)
			if (idmap.at(i) == aid)
				identity_->setCurrentIndex(i);

		info_messages_->addWidget(make_unique<WText>(WString(
			"<p>Hello, {1}, your session id is: {2}.</p>")
				.arg(identity_->currentText())
				.arg(aid)));
		if (on_new_sid) on_new_sid(sid, aid);
	}
}

void identities::submit() {
	info_messages_->clear();
	error_messages_->clear();
	set_agent(idmap[identity_->currentIndex()]);
}

}
