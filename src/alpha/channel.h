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
#include <sstream>

#include "defs.h"
#include "message.h"

namespace alpha {

struct channel : object {
	channel_id id;
	agent_id creator;
	std::string name;
	timestamp created;
	channel() = default;
	channel(std::string name) : name(name) {}
	channel(strings e) {
		std::stringstream ss;
		switch (e.size()) {
			case 4: ss.str(e[3]); ss >> created;
				/* fall through */
			case 3: name = e[2];
				/* fall through */
			case 2: creator = e[1];
				/* fall through */
			case 1: id = e[0];
		}
	};
};

std::ostream& operator<<(std::ostream& os, const channel& ch);

typedef std::shared_ptr<channel> sp_channel;
typedef std::unique_ptr<channel> up_channel;

}

#endif
