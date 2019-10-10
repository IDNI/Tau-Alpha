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
#ifndef __ALPHA_CHANNEL_H__
#define __ALPHA_CHANNEL_H__

#include "defs.h"
#include "message.h"

namespace alpha {

struct channel {
	channel_id id_;
	agent_id op_;
	std::string title_;
	timestamp created_;
	agent_id op() const { return op_; }
	std::string title() const { return title_; }
	timestamp created() const { return created_; }
	std::string id() const { return id_; }
	std::vector<message> messages() {
		return std::vector<message>{
			message{"0", "john", { "general" }, "Hello from John",
							"... content ..."},
			message{"1", "john", { "general" }, "Hello from John2",
							"... content2 ..."}
		};
	}
};

}

#endif
