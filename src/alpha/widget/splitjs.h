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
#ifndef __WT_SPLIT_JS_H__
#define __WT_SPLIT_JS_H__

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>

namespace alpha {

namespace widget {

class splitjs : public Wt::WContainerWidget {
public:
	enum direction { VERTICAL, HORIZONTAL };
	splitjs(direction d = VERTICAL, std::string s = "50,50") :
		direction_(d),
		sizes_(s)
	{
		Wt::WApplication* app = Wt::WApplication::instance();
		app->require(app->resolveRelativeUrl(
			"resources/splitjs/split.min.js"));
		app->useStyleSheet(app->resolveRelativeUrl(
			"resources/splitjs/splitjs.css"));
		this->addStyleClass(direction_ == VERTICAL
			? "flex-column"
			: "flex-row");
		first_  = this->addWidget(
				std::make_unique<Wt::WContainerWidget>());
		second_ = this->addWidget(
				std::make_unique<Wt::WContainerWidget>());
		std::stringstream js; js
			<< "Split(['#"
			<< first_->id() << "', '#" << second_->id()
			<< "'], {\n"
			<< "\tsizes: [ " << sizes_ << " ],\n"
			<< "\tminSize: 0,\n"
			<< "\tdirection: '"
			<< (direction_ == VERTICAL ? "vertical" : "horizontal")
			<< "',\n"
			<< "\tcursor: '"
			<< (direction_ == VERTICAL ? "row" : "col")
			<< "-resize',\n"
			<< "})\n";
		DBG(Wt::log("info")<<js.str();)
		this->doJavaScript(js.str());
	}
	std::string direction_string() const {
		return direction_ == VERTICAL ? "vertical" : "horizontal";
	}
	Wt::WContainerWidget *first() { return first_; }
	Wt::WContainerWidget *second() { return second_; }
private:
	direction direction_;
	std::string sizes_;
	Wt::WContainerWidget *first_;
	Wt::WContainerWidget *second_;
};

}

}
#endif
