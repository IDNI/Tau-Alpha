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

#include "channels.h"
#include "../../protocol.h"
#include "../app.h"

namespace alpha::wt::view {

using namespace Wt;
using std::make_unique;
using std::cout;
using std::endl;

channels::channels(channel_id chid, on_new_channel_id onnew,
	on_select_channel_id onsel)
:
	WTable()
	, chid(chid)
	, on_new_chid(onnew)
	, on_select_chid(onsel)
{
	create_ui();
}

void channels::set_channel_id(session_id newchid) {
	chid = newchid;
	update_list();
}

void channels::create_ui() {
	WLabel *label;
	int row = 0;

	elementAt(row, 0)->setColumnSpan(3);
	elementAt(row, 0)->setContentAlignment(AlignmentFlag::Top | AlignmentFlag::Center);
	elementAt(row, 0)->setPadding(10);
	WText *title = elementAt(row,0)->addWidget(
		make_unique<WText>(tr("select channel")));
	title->decorationStyle().font().setSize(FontSize::XLarge);

	// channel list
	++row;
	list_ = elementAt(row,2)->addWidget(make_unique<WContainerWidget>());

	// create channel
	++row;
	elementAt(row, 0)->setColumnSpan(3);
	elementAt(row, 0)->setContentAlignment(AlignmentFlag::Top | AlignmentFlag::Center);
	elementAt(row, 0)->setPadding(10);
	WText *title2 = elementAt(row,0)->addWidget(
		make_unique<WText>(tr("create channel")));
	title2->decorationStyle().font().setSize(FontSize::XLarge);

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

	// name
	++row;
	name_ = elementAt(row,2)->addWidget(make_unique<WLineEdit>());
	label = elementAt(row,0)->addWidget(
		make_unique<WLabel>(tr("label.name")));
	label->setBuddy(name_);
	auto validator = std::make_shared<Wt::WRegExpValidator>(
		"[a-zA-Z0-9_+-/\\*\\., ]+");
	name_->setValidator(validator);
	name_->enterPressed().connect(this, &channels::submit);
	name_->setFocus(true);

	// submit
	++row;
	WPushButton *submit = elementAt(row,0)->addWidget(
		make_unique<WPushButton>(tr("submit")));
	submit->clicked().connect(this, &channels::submit);
	submit->setMargin(15, Side::Top);
	elementAt(row, 0)->setColumnSpan(3);
	elementAt(row, 0)->setContentAlignment(
		AlignmentFlag::Top | AlignmentFlag::Center);

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

bool channels::check_valid(WFormWidget *edit, const WString& text) {
	if (edit->validate() != ValidationState::Valid) {
		error_messages_->addWidget(
			make_unique<WText>(text));
		error_messages_->addWidget(
			make_unique<WBreak>());
		edit->label()
			->decorationStyle().setForegroundColor(WColor("red"));
		edit->setStyleClass("Wt-invalid");
		return false;
	} else {
		edit->label()->decorationStyle().setForegroundColor(WColor());
		edit->setStyleClass("");
		return true;
	}
}

bool channels::validate() {
	error_messages_->clear();
	bool valid = true;
	if (!check_valid(name_, tr("error.name"))) valid = false;
	return valid;
}

void channels::submit() {
	info_messages_->clear();
	error_messages_->clear();
	if (validate()) {
		alpha::channel ch(name_->text().toUTF8());
		bool r = protocol::create_channel(
			app::instance()->get_session_id(), ch);
		if (!r) {
			error_messages_->addWidget(make_unique<WText>(
				"create channel failed: " +
				name_->text().toUTF8()));
			error_messages_->addWidget(make_unique<WBreak>());
		} else {
			chid = ch.id;
			info_messages_->addWidget(make_unique<WText>(WString(
				"<p>Channel {1}, created. ID: {2}.</p>")
					.arg(name_->text())
					.arg(chid)));
			if (on_new_chid) on_new_chid(chid);
			update_list();
		}
	}
}

void channels::update_list() {
	cout<<"update_list: current channel id: "<<chid<<endl;
	auto chans = app::instance()->channels()->get_list(channel_f{});
	list_->clear();
	auto fn = [=](channel_id newchid) -> auto {
		return [=]() {
			chid = newchid;
			if (on_select_chid) on_select_chid(chid);
			//std::cout << "clicked channel: " << chid << std::endl;
			update_list();
		};
	};
	//if (!chans.size()) std::cout << "no channels found." << std::endl;
	for (auto& ch : chans) {
		auto div = list_->addWidget(make_unique<WContainerWidget>());
		div->addWidget(make_unique<WText>(WString("[{1}] {2} by ")
							.arg(ch->id)
							.arg(ch->name)));
		auto c = app::instance()->agents()->get(ch->creator).get();
		div->addWidget(make_unique<view::agent>(
				view::agent::render_type::SHORT, c));
		div->addWidget(make_unique<WText>(WString(" from {1}")
						.arg(print_date(ch->created))));
		div->clicked().connect(fn(ch->id));
		if (chid != "" && chid == ch->id) {
			WCssDecorationStyle& style = div->decorationStyle();
			style.font().setWeight(FontWeight::Bold);
			style.setForegroundColor(WColor("green"));
		}
		list_->addWidget(make_unique<WBreak>());
	}
}

}
