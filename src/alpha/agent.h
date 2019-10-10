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
#ifndef __ALPHA_AGENT_H__
#define __ALPHA_AGENT_H__

#include "defs.h"

namespace alpha {

struct agent {
	agent_id         id_;
//	cryptographic_id identity_;
//	picture          picture_;
	std::string      name_;       // username
	std::string      other_name_; // real/other name
//	std::string      email_;
//	std::string      about_;
//	custom_ids       custom_ids_;
	agent_id id() { return id_; }
	std::string name() { return name_; }
	std::string other_name() { return other_name_; }
};

}

#endif
