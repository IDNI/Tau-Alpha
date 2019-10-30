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
#ifndef __ALPHA_WT_VIEW_AGENT_H__
#define __ALPHA_WT_VIEW_AGENT_H__
#include <Wt/WTemplate.h>

#include "../../agent.h"

namespace alpha::wt::view {

struct agent : public Wt::WTemplate {
	enum render_type { SHORT, DETAIL, EDIT };
	strings type_names = { "short", "detail", "edit" };
	sp_agent get_agent(const agent_id& aid);
	agent(const render_type& type, const agent_id& aid);
	agent(const render_type& type, sp_agent sa=0);
	void render();
	void bind_data();
private:
	render_type type;
	sp_agent sa;
};

}
#endif
