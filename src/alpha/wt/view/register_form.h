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
#ifndef __ALPHA_WT_VIEW_REGISTER_FORM_H__
#define __ALPHA_WT_VIEW_REGISTER_FORM_H__

#include <Wt/WTable.h>
#include <Wt/WFormWidget.h>
#include <Wt/WString.h>
#include <Wt/WLineEdit.h>
#include <Wt/WContainerWidget.h>

#include "../../agent.h"

namespace alpha::wt::view {

class register_form : public Wt::WTable {
public:
	register_form();
	void submit();
	void create_ui();

private:
	std::unique_ptr<agent> agent_;
	Wt::WContainerWidget *info_messages_;
	Wt::WContainerWidget *error_messages_;
	Wt::WLineEdit *name_;
	Wt::WLineEdit *other_name_;
	void add_validation_status(int row, Wt::WFormWidget *field);
	bool validate();
	bool check_valid(Wt::WFormWidget *edit, const Wt::WString& text);
};

}
#endif
