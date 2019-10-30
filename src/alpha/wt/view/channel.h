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
#ifndef __ALPHA_WT_VIEW_CHANNEL_H__
#define __ALPHA_WT_VIEW_CHANNEL_H__
#include <Wt/WTemplate.h>

#include "../../channel.h"

namespace alpha::wt::view {

struct channel : public Wt::WTemplate {
	channel(sp_channel sch);
	void render();
	void bind_data();
	void bind_messages();
private:
	sp_channel sch;
	message* post_form;
};

}
#endif
