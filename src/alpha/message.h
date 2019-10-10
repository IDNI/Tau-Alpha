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
#ifndef __ALPHA_MESSAGE_H__
#define __ALPHA_MESSAGE_H__

#include "defs.h"

namespace alpha {

struct message {
	message_id id_;
	agent_id author_;
	std::vector<channel_id> targets_;
	std::string title_;
	file content_;
//	file formal_;
//	timestamp created_;
//	timestamp publish_;
//	timestamp valid_;
//	std::vector<message_id> reacts_to_;
	agent_id author() const { return author_; }
	std::string title() const { return title_; }
	std::string content() const { return content_; }
};

}

#endif
