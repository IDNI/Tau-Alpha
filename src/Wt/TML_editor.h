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
#ifndef __WT_TML_EDITOR_H__
#define __WT_TML_EDITOR_H__

#include "codemirror.h"

namespace Wt {

class TML_editor : public codemirror {
public:
	TML_editor() {
		WApplication* app = WApplication::instance();
		app->require(app->resolveRelativeUrl(
			"resources/codemirror_grammar/"
						"codemirror_grammar.min.js"));
		app->require(app->resolveRelativeUrl(
			"resources/codemirror_grammar/tml_grammar.js"));
		std::stringstream js; js
			<< "var tml_mode ="
			<< " CodeMirrorGrammar.getMode(tml_grammar);\n"

			// << "tml_modesupportGrammarAnnotations = true;\n"
			// << "tml_mode.supportCodeFolding = true;\n"
			// << "tml_mode.supportCodeMatching = true;\n"
			// << "tml_mode.supportAutoCompletion = true;\n"
			// << "tml_mode.autocompleter.options = {prefixMatch:true,"
			// 	<< "caseInsensitiveMatch:false,inContext:true,"
			// 	<< "dynamic:true};\n"

			// << "CodeMirror.registerHelper('lint', 'tml', tml_mode.linter);"
			// << "CodeMirror.registerHelper('fold', "
			// 	<< "tml_mode.foldType, tml_mode.folder);\n"

			// << "tml_mode.matcher.options = "
			// 	<< "{maxHighlightLineLength:1000};\n"
			// << "CodeMirror.defineOption('matching', false, "
			// 	<< "function(cm, val, old) {\n"
			// << "\tif (old && old != CodeMirror.Init) {\n"
			// << "\t\tcm.off('cursorActivity', tml_mode.matcher);\n"
			// << "\t\ttml_mode.matcher.clear(cm);\n"
			// << "\t}\n"
			// << "\tif (val) {\n"
			// << "\t\tcm.on('cursorActivity', tml_mode.matcher);\n"
			// << "\t\ttml_mode.matcher(cm);\n"
			// << "\t}\n"
			// << "});\n"

			// << "var autocomplete_cmd = 'autocomplete_grammar_tml',"
			// << "togglecomment_cmd = 'togglecomment_grammar_tml';\n"
			// << "CodeMirror.commands[autocomplete_cmd]=function(cm){"
			// << "\tCodeMirror.showHint(cm,tml_mode.autocompleter);\n"
			// << "};\n"

			<< "CodeMirror.defineMode('tml', tml_mode);\n";

			// << jsRef()<<".cm.setOption(tml_mode.matchType,true);\n"
			// << jsRef()<<".cm.setOption('extraKeys', { "
			// 	<< "'Ctrl-Space': autocomplete_cmd, "
			// 	<< "'Ctrl-L': 'toggleComment'"
			// 	<< "});\n";
		DBG(Wt::log("info")<<js.str();)
		this->doJavaScript(js.str());
		setOption("mode", "'tml'");
		setOption("indentUnit", "8");
		setOption("indentWithTabs", "true");
		// setOption("matching", "true");
		// setOption("foldGutter", "true");
		// setOption("lint", "true");
		// setOption("gutters", "['CodeMirror-lint-markers', "
		// 				"'CodeMirror-linenumbers', "
		// 				"'CodeMirror-foldgutter']");
		setSize("100%", "100%");
	};
};

}
#endif
