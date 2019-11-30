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

#include <Wt/WCssDecorationStyle.h>
#include <Wt/WColor.h>

#include "register_form.h"
#include "../../../tml/relationfile.h"
#include "../../protocol.h"

namespace alpha::wt::view {

using namespace Wt;
using std::make_unique;

register_form::register_form() : WTable() {
	create_ui();
}

void register_form::create_ui() {
	WLabel *label;
	int row = 0;

	// Title
	elementAt(row, 0)->setColumnSpan(3);
	elementAt(row, 0)->setContentAlignment(AlignmentFlag::Top | AlignmentFlag::Center);
	elementAt(row, 0)->setPadding(10);
	WText *title = elementAt(row,0)->addWidget(
		make_unique<WText>(tr("register form")));
	title->decorationStyle().font().setSize(FontSize::XLarge);

	// error messages
	++row;
	elementAt(row, 0)->setColumnSpan(3);
	error_messages_ = elementAt(row, 0);//messages_->addWidget(make_unique<WContainerWidget>());
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
	name_->enterPressed().connect(this, &register_form::submit);
	name_->setFocus(true);

	// other name
	++row;
	other_name_ = elementAt(row,2)->addWidget(
		make_unique<WLineEdit>());
	label = elementAt(row,0)->addWidget(
		make_unique<WLabel>(tr("label.other_name")));
	label->setBuddy(other_name_);
	other_name_->enterPressed().connect(this, &register_form::submit);

	// submit
	++row;
	WPushButton *submit = elementAt(row,0)->addWidget(
		make_unique<WPushButton>(tr("submit")));
	submit->clicked().connect(this, &register_form::submit);
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

bool register_form::check_valid(WFormWidget *edit, const WString& text) {
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

bool register_form::validate() {
	error_messages_->clear();
	bool valid = true;
	if (!check_valid(name_, tr("error.name"))) valid = false;
	return valid;
}

void register_form::submit() {
	info_messages_->clear();
	error_messages_->clear();
	if (validate()) {
		agent a(name_->text().toUTF8(), other_name_->text().toUTF8());
		bool r = protocol::register_agent(a);
		if (!r) {
			error_messages_->addWidget(make_unique<WText>(
				"registration failed (choose unique name)"));
			error_messages_->addWidget(make_unique<WBreak>());
		} else {
			tml::relationfile identity("identity", 3,
				"storage/identity.tml",
				"storage/common.tml");
			identity.erase(a.id);
			identity.insert(a.id,
				name_->text().toUTF8(),
				other_name_->text().toUTF8());
			info_messages_->addWidget(make_unique<WText>(WString(
				"<p>Hello, {1}, you've been registered with"
				" id: {2}.</p>")
					.arg(name_->text()).arg(a.id)));
			name_->setText(WString());
			other_name_->setText(WString());
		}
	}
}

}
