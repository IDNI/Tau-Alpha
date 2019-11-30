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
#include <sstream>

#include "defs.h"

namespace alpha {

struct message : public object {
	message_id id;
	agent_id author;
	channel_ids targets;
	std::string subject;
	file content;
//	file formal;
	timestamp created;
//	timestamp publish;
//	timestamp valid;
//	message_ids reacts_to;
	message() = default;
	message(strings e) {
		std::stringstream ss;
		switch (e.size()) {
			case 6: ss.str(e[5]); ss >> created;
				/* fall through */
			case 5: content = e[4];
				/* fall through */
			case 4: subject = e[3];
				/* fall through */
			case 3: targets = split(e[2]);
				/* fall through */
			case 2: author = e[1];
				/* fall through */
			case 1: id = e[0];
		}
	};

};

std::ostream& operator<<(std::ostream& os, const message& m);

typedef std::shared_ptr<message> sp_message;

}

#endif
