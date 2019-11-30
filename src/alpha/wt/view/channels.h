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
#ifndef __ALPHA_WT_VIEW_CHANNELS_H__
#define __ALPHA_WT_VIEW_CHANNELS_H__

#include <Wt/WTable.h>
#include <Wt/WFormWidget.h>
#include <Wt/WString.h>
#include <Wt/WLineEdit.h>
#include <Wt/WContainerWidget.h>

#include "../../agent.h"
#include "../../../tml/relationfile.h"
#include "../../config.h"

namespace alpha::wt::view {

using on_new_channel_id    = std::function<void(channel_id)>;
using on_select_channel_id = std::function<void(channel_id)>;

class channels : public Wt::WTable {
public:
	channels(channel_id chid, on_new_channel_id on_new_chid=0,
			on_select_channel_id on_select_chid=0);
	void submit();
	void create_ui();
	void set_on_new_channel_id(on_new_channel_id e) { on_new_chid = e; }
	void set_on_select_channel_id(on_select_channel_id e) {
							on_select_chid = e; }
	void set_channel_id(channel_id newchid);
	channel_id get_channel_id() { return chid; }
	void update_list();
private:
	channel_id chid;
	on_new_channel_id on_new_chid=0;
	on_select_channel_id on_select_chid=0;
	Wt::WContainerWidget *info_messages_;
	Wt::WContainerWidget *error_messages_;
	Wt::WLineEdit *name_;
	Wt::WContainerWidget *list_;
	void add_validation_status(int row, Wt::WFormWidget *field);
	bool validate();
	bool check_valid(Wt::WFormWidget *edit, const Wt::WString& text);
};

}

#endif
