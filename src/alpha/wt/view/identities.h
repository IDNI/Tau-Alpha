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
#ifndef __ALPHA_WT_VIEW_IDENTITIES_H__
#define __ALPHA_WT_VIEW_IDENTITIES_H__

#include <Wt/WTable.h>
#include <Wt/WFormWidget.h>
#include <Wt/WString.h>
#include <Wt/WLineEdit.h>
#include <Wt/WContainerWidget.h>

#include "../../agent.h"
#include "../../../tml/relationfile.h"

namespace alpha::wt::view {

using on_new_session_id = std::function<void(session_id, agent_id)>;

class identities : public Wt::WTable {
public:
	identities(tml::relationfile db, on_new_session_id on_new_sid=0);
	void submit();
	void create_ui();
	void set_db(tml::relationfile ndb) { db = ndb; }
	void set_agent(const agent_id& aid);
	void set_on_new_session_id(on_new_session_id e) { on_new_sid = e; }
private:
	agent_id aid;
	tml::relationfile db;
	on_new_session_id on_new_sid=0;
	strings idmap{};
	Wt::WContainerWidget *info_messages_;
	Wt::WContainerWidget *error_messages_;
	Wt::WSelectionBox *identity_;
};

}

#endif
