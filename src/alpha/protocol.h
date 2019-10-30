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
#ifndef __ALPHA_PROTOCOL_H__
#define __ALPHA_PROTOCOL_H__

#include "defs.h"
#include "agent.h"
#include "message.h"
#include "channel.h"
#include "filter.h"

namespace alpha {

namespace protocol {

typedef void (*on_notify)(message_ids&);

bool register_agent(agent& a);
//bool unregister_agent(const session_id& sid, agent& a); // TODO
session_id login(agent_id aid, std::string password);
bool logout  (const session_id& sid);
//bool notify  (const session_id& sid, filter::notification& n, on_notify fn);
//bool unnotify(const session_id& sid, filter::notification& n); // TODO
//bool ignore  (const session_id& sid, filter::ignoration& i);   // TODO
//bool unignore(const session_id& sid, filter::ignoration& i);   // TODO
bool send    (const session_id& sid, message& m);
//bool unsend  (const session_id& sid, message& m);              // TODO
//bool update  (const session_id& sid, message& m);              // TODO
bool create_channel(const session_id& sid, channel& ch);

//bool list_notifications(const session_id& sid, std::vector<filter::notification>& r);
//bool list_ignorations  (const session_id& sid, std::vector<filter::ignoration>&   r);

template<typename T>
unique_ids query(const session_id& sid, const filter& f);
template <typename T>
std::vector<T> fetch(const session_id &sid, std::vector<unique_id> ids);

}

}

#endif
