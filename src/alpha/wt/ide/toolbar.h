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
#ifndef __ALPHA_WT_IDE_TOOLBAR_H__
#define __ALPHA_WT_IDE_TOOLBAR_H__

#include <Wt/WToolBar.h>
#include <Wt/WPushButton.h>

namespace alpha::wt::ide {

typedef std::function<void()> on_click;

struct toolbar : public Wt::WToolBar {
	toolbar(const on_click& runtime_backend, const on_click& runtime_frontend) {
#ifndef DISABLE_BACKEND_EXECUTION
		auto bbtn = std::make_unique<Wt::WPushButton>(tr("RUN"));
		bbtn->clicked().connect([runtime_backend](){
							runtime_backend(); });
		run_backend_btn_ = bbtn.get();
		addButton(move(bbtn));
#endif
#ifndef DISABLE_FRONTEND_EXECUTION
		auto fbtn = std::make_unique<Wt::WPushButton>(tr("RUN JS"));
		fbtn->clicked().connect([runtime_frontend](){
							runtime_frontend(); });
		run_frontend_btn_ = fbtn.get();
		addButton(move(fbtn));
#endif
	}
	Wt::WPushButton* run_backend_btn_;
	Wt::WPushButton* run_frontend_btn_;
};

}
#endif
