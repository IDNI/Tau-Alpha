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
#ifndef __WT_CODEMIRROR_H__
#define __WT_CODEMIRROR_H__

#include <Wt/WApplication.h>
#include <Wt/WTextArea.h>
#include <Wt/WContainerWidget.h>
#include <string>
#include <sstream>

namespace Wt {

class codemirror : public WContainerWidget {
	WTextArea * textArea_;
	JSignal<std::string> onUpdate_;
public:
	codemirror() : onUpdate_(this, "onUpdate") {
		WApplication* app = WApplication::instance();
		app->require(app->resolveRelativeUrl(
			"resources/CodeMirror/lib/codemirror.js"));
		// app->require(app->resolveRelativeUrl(
		// 	"resources/CodeMirror/addon/edit/matchbrackets.js"));
		app->require(app->resolveRelativeUrl(
			"resources/CodeMirror/addon/comment/comment.js"));
		// app->require(app->resolveRelativeUrl(
		// 	"resources/CodeMirror/addon/lint/lint.js"));
		// app->require(app->resolveRelativeUrl(
		// 	"resources/CodeMirror/addon/hint/show-hint.js"));
		// app->require(app->resolveRelativeUrl(
		// 	"resources/CodeMirror/addon/fold/foldcode.js"));
		// app->require(app->resolveRelativeUrl(
		// 	"resources/CodeMirror/addon/fold/foldgutter.js"));
		app->useStyleSheet(app->resolveRelativeUrl(
			"resources/CodeMirror/lib/codemirror.css"));
		// app->useStyleSheet(app->resolveRelativeUrl(
		// 	"resources/CodeMirror/addon/lint/lint.css"));
		// app->useStyleSheet(app->resolveRelativeUrl(
		// 	"resources/CodeMirror/addon/hint/show-hint.css"));
		// app->useStyleSheet(app->resolveRelativeUrl(
		// 	"resources/CodeMirror/addon/fold/foldgutter.css"));
		textArea_ = this->addWidget(std::make_unique<WTextArea>());
		textArea_->textInput().connect([this]{
			std::stringstream js; js
				<< "setTimeout (function() {\n"
				<< jsRef() << ".cm.getDoc().setValue("
				<< jsValue() << ");\n"
				<< "}, 0);\n";
			DBG(Wt::log("info")<<js.str();)
			this->doJavaScript(js.str());
		});
		std::stringstream js; js
			<< "var e = " << textArea_->jsRef() << ";\n"
			<< "var self = " << jsRef() << ";\n"
			<< "self.updateTimeout = 0;\n"
			<< "self.cm = CodeMirror.fromTextArea(e, {});\n"
			<< "self.cm.getDoc().on('change',function(cm,chObj) {\n"
			<< "\tclearTimeout(self.updateTimeout);\n"
			<< "\tself.cm.getTextArea().value = cm.getValue(); \n"
			<< "\tself.updateTimeout = "
				<< "setTimeout(debouncedUpdate, 200); \n"
			<< "\tfunction debouncedUpdate() {\n"
			<< "\t\t" << textArea_->jsRef() << ".value = cm.getValue();\n"
			<< "\t\t"<<onUpdate_.createCall({"cm.getValue()"})<<"\n"
			<< "\t}\n"
			<< "});\n"
 			<< "self.cm.setOption(\"theme\", \"tml\");\n";
		DBG(Wt::log("info")<<js.str();)
		this->doJavaScript(js.str());
	}

	void setOption(std::string name, std::string value) {
		std::stringstream js;
		js<<jsRef()<<".cm.setOption('"<<name<<"', "<<value<<");\n";
		DBG(Wt::log("info")<<js.str();)
		this->doJavaScript(js.str());
	}

	void setSize(std::string width, std::string height) {
		this->resize(width, height);
		std::stringstream js;
		js<<jsRef()<<".cm.setSize('"<<width<<"', '"<<height<<"');\n";
		DBG(Wt::log("info")<<js.str();)
		this->doJavaScript(js.str());
	}

	void setText(std::string text) {
		std::stringstream js;
		textArea_->setValueText(text);
		js << "setTimeout (function() {"
			<< jsRef() << ".cm.getDoc().setValue("
			<< jsValue() << ");"
			<< "}, 0);\n";
		DBG(Wt::log("info")<<js.str();)
		this->doJavaScript(js.str());
	}

	std::string jsValue() {
		std::stringstream js;
		js << textArea_->jsRef() << ".value";
		return js.str();
	}

	std::string getText() {
		return textArea_->valueText().toUTF8();
	}

	JSignal<std::string>& onUpdate() { return onUpdate_; }
};

}
#endif
