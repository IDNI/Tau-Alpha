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
#ifndef __ALPHA_DEFS_H__
#define __ALPHA_DEFS_H__
#include <stdint.h>
#include <string>
#include <vector>
#include <array>

namespace alpha {

typedef time_t timestamp;
typedef std::array<timestamp, 2> timerange;
typedef std::string file;
typedef std::string unique_id;
typedef std::string cryptographic_id;

typedef unique_id agent_id;
typedef unique_id message_id;
typedef unique_id channel_id;
typedef file picture;                 // or IRI? (file:///home/user/pic.jpg)
typedef file custom_id;               // or IRI?
typedef std::vector<custom_id> custom_ids;

}

#endif
