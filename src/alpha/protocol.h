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
#include "defs.h"
#include "agent.h"
#include "message.h"
#include "channel.h"
#include "filter.h"

namespace alpha {

namespace protocol {

typedef void (*on_notify)(std::vector<message_id>&);

struct session {
	agent a;
};

bool register_agent(agent& a);
bool unregister_agent(session& s, agent& a);
session login(agent& a);
bool logout  (session& s);
bool notify  (session& s, filter::notification& n, on_notify fn);
bool unnotify(session& s, filter::notification& n);
bool ignore  (session& s, filter::ignoration& i);
bool unignore(session& s, filter::ignoration& i);
bool send    (session& s, message& m);
bool unsend  (session& s, message& m);
bool update  (session& s, message& m);

bool query_channels(session& s, filter::channel& f, std::vector<channel>& r);
bool query_messages(session& s, filter::message& f, std::vector<message>& r);
bool query_agents  (session& s, filter::agent&   f, std::vector<agent>&   r);
bool list_notifications(session& s, std::vector<filter::notification>& r);
bool list_ignorations  (session& s, std::vector<filter::ignoration>&   r);

}

}
