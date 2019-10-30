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

struct agent : public object {
	agent_id         id{0};
//	cryptographic_id identity;
//	picture          picture;
	std::string      name;       // username
	std::string      password;
	std::string      other_name; // real/other name
//	std::string      email;
//	std::string      about;
//	custom_ids       custom_ids;
	agent() = default;
	agent(std::string name, std::string password, std::string other_name="")
		: name(name), password(password), other_name(other_name) {};
};

std::ostream& operator<<(std::ostream& os, const agent& a);

typedef std::shared_ptr<agent> sp_agent;

}

#endif
