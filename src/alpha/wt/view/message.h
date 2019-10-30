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
#ifndef __ALPHA_WT_VIEW_MESSAGE_H__
#define __ALPHA_WT_VIEW_MESSAGE_H__
#include <functional>

#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>
#include <Wt/WTextArea.h>
#include <Wt/WLineEdit.h>

#include "../../message.h"

namespace alpha::wt::view {

typedef std::function<void()> on_send_fn_t;

struct message : public Wt::WTemplate {
	enum render_type { SHORT, DETAIL, FORM };
	message(const render_type& type, sp_message sm=0, on_send_fn_t cb=0);
	void render();
	void render_form();
	void bind_data();
	void send();
private:
	render_type type;
	sp_message sm;
	on_send_fn_t cb_;
	strings type_names = { "short", "detail", "form" };
	Wt::WLineEdit *subject_;
	Wt::WTextArea *content_;
	Wt::WPushButton *send_;
};

}
#endif
