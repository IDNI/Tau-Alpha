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
#ifndef __ALPHA_WT_IDE_MENU_H__
#define __ALPHA_WT_IDE_MENU_H__

#include <Wt/WToolBar.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>

namespace alpha::wt::ide {

//using on_change = void (*)(Wt::WMenuItem*);

typedef std::function<void(Wt::WMenuItem *)> on_change;

struct menu : public Wt::WToolBar {
	menu(bool ln, on_change on_ln,
		bool ar, on_change on_ar) : Wt::WToolBar()
	{
		auto sm = std::make_unique<Wt::WPopupMenu>();

		auto s_ln = sm->addItem(tr("show line numbers"));
		s_ln->setCheckable(true);
		s_ln->setChecked(ln);
		s_ln->triggered().connect([on_ln, s_ln](){ on_ln(s_ln); });

		auto s_ar = sm->addItem(tr("autorun"));
		s_ar->setCheckable(true);
		s_ar->setChecked(ar);
		s_ar->triggered().connect([on_ar, s_ar](){ on_ar(s_ar); });

		auto sb = std::make_unique<Wt::WPushButton>(tr("settings"));
		sb->setMenu(std::move(sm));
		addButton(std::move(sb));
	}
};

}
#endif
