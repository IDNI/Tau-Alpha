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
var tml_grammar = {
	"RegExpID": "RE::",
	"Extra": { "fold": 'braces' },
	"Style": {
		 "comment":              "comment"
		,"string":               "string"
		,"open_paren":           "open_paren"
		,"close_paren":          "close_paren"
		,"open_brace":           "open_brace"
		,"close_brace":          "close_brace"
		,"pipe":                 "alt"
		,"tilde":                "not"
		,"dot":                  "dot"
		,"comma":                "and"
		,"excl_mark":            "query"
		,"delimiter":            "delimiter"
		,"prod_delimiter":       "production delimiter"
		,"variable":             "variable"
		,"symbol":               "symbol"
		,"cmdline":              "cmdline"
		,"number":               "number"
		,"char":                 "char"
		,"fname":                "filename"
		,"atom":                 "atom"
		,"keyword":              "keyword"
		,"relname":              "relation"
		,"fact_relname":         "fact relation"
		,"query_relname":        "query relation"
		,"head_relname":         "head relation"
		,"body_relname":         "body relation"
		,"prod_relname":         "production relation"
		,"str_relname":          "directive relation"
	},
	"Lex": {
		"comment:comment": {
			"interleave": true,
			"tokens": [ [  "#",  null ] ]
		}
		,"string:escaped-block": [ '"', '"' ]
		,"open_paren":           "("
		,"close_paren":          ")"
		,"open_brace":           "{"
		,"close_brace":          "}"
		,"pipe":                 "|"
		,"tilde":                "~"
		,"dot":                  "."
		,"comma":                ","
		,"excl_mark":            "!"
		,"delimiter":            ":-"
		,"prod_delimiter":       "=>"

		// lookaheads to distinguish production, rule and fact
		,"LAdelimiter":          "RE::/(?:(?!:-)[^\.])+:-/"
		,"LAprod_delimiter":     "RE::/(?:(?!=>)[^\.])+=>/"

		,"variable":             "RE::/\\?[a-zA-Z][_a-zA-Z0-9]*/"
		,"symbol":               "RE::/[a-zA-Z][_a-zA-Z0-9]*/"
		,"cmdline":              "RE::/\\$\\d+/"
		,"number":               "RE::/\\d+/"
		,"char":                 "RE::/'\\\\?.?'/"
		,"fname":                "RE::/<[^>]*>/"
		,"atom": {
			"autocomplete": true,
			"meta": "TML Atom",
			"tokens": [ "null" ]
		}
		,"keyword": {
			"autocomplete": true,
			"meta": "TML Keyword",
			"tokens": [
				"\@string", "\@stdout", "\@trace", "\@bwd"
			]
		}
	},
	"Syntax": {
		 "fact":        "fact_term dot"
		,"rule":        "head delimiter body dot"
		,"head":        "head_term (comma head_term)*"
		,"body":        "body_term (comma body_term)*"

		,"production":  "relname.prod_relname prod_delimiter alts dot"
		,"alts":        "alt* ( pipe alts )*"
		,"alt":         "string | char | null.atom | " +
					"relname.prod_relname"

		,"query":       "excl_mark{1,2} query_term dot"

		,"directive":   "\@& ( \@string.keyword strdir | " +
					"\@stdout.keyword term | " +
					"\@trace.keyword symbol | " +
					"\@bwd.keyword ) dot"
		,"strdir":      "relname.str_relname " +
					"( string | fname | cmdline | term )"

		,"term":        "tilde? relname args*"
		,"fact_term":   "tilde? relname.fact_relname args*"
		,"query_term":  "tilde? relname.query_relname args*"
		,"body_term":   "tilde? relname.body_relname args*"
		,"head_term":   "tilde? relname.head_relname args*"

		,"args":        "( open_paren args close_paren | identifier )*"

		,"relname":     "symbol"
		,"identifier":  "char | variable | symbol | number"
		,"statement":   "directive | query | "+
				"LAprod_delimiter& production | " +
				"LAdelimiter& rule | " +
				"fact"
		,"tml":         "( open_brace statement* close_brace | " +
								"statement*)"
	},
	"Parser": [ "comment", "string", [ "tml" ] ]
};
