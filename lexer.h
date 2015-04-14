/*
 * lexer.h
 *
 *  Created on: Apr 7, 2015
 *      Author: danny
 */

#ifndef LEXER_H_
#define LEXER_H_

#include "keywords.h"
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>


#define DISSEQT_LEXER_INITIALIZER( r, data, keyword) , keyword( "(?i:" BOOST_PP_STRINGIZE(keyword) ")")
#define DISSEQT_LEXER_DECLARE_MEMBER( r, type, keyword) type keyword;
#define DISSEQT_LEXER_BARE_PREFIX( r, prefix, keyword)  prefix keyword

namespace disseqt {
    template< typename L>
    struct Lexer : boost::spirit::lex::lexer<L>
    {
        typedef boost::spirit::lex::token_def<> void_token_def;
        typedef boost::spirit::lex::token_def<std::string> string_token_def;
        Lexer()
        :WHITESPACE("\\s+"),
         COMMENT(R"(\/\*[^*]*\*+([^/*][^*]*\*+)*\/|\/\/.*$)"),
         EQ_OP("=="),
         NEQ_OP("!=|<>"),
         LE_OP("<="),
         GE_OP(">="),
         SHLEFT_OP("<<"),
         SHRIGHT_OP(">>"),
         CONCAT_OP("\\|\\|"),
         UNKNOWN("."),
         NULL_T("(?s:NULL)")
         BOOST_PP_SEQ_FOR_EACH( DISSEQT_LEXER_INITIALIZER, _, DISSEQT_KEYWORDS)
        {
            this->self.add_pattern
                        ("DOUBLE_QUOTED", R"('[^'\\]*(\\.[^'\\]*)*')")
                        ("SINGLE_QUOTED", R"(\"[^"\\]*(\\.[^"\\]*)*\")")
                        ("IDENT",         R"([a-zA-Z_][a-zA-Z0-9_]*)")
                        ("HEX",           R"(0[xX][0-9a-fA-F]+)")
                        ("NUM",           R"((\d+(\.\d*)?|\.\d+)([eE]\d+)?)")
                        ;
            STRING      = "{DOUBLE_QUOTED}|{SINGLE_QUOTED}";
            BLOB        = "[Xx]({DOUBLE_QUOTED}|{SINGLE_QUOTED})";
            IDENTIFIER  = "{IDENT}";
            BIND_PARAMETER  = "\\?\\d*|:{IDENT}|@{IDENT}|\\${IDENT}(::{IDENT}?)*(\\([^)]*\\))?";
            NUMERIC_LITERAL = "{HEX}|{NUM}";


            this->self.add
                DISSEQT_KEYWORDS
                (NULL_T)
                (BLOB)
                (IDENTIFIER)
                (BIND_PARAMETER)
                (NUMERIC_LITERAL)
                (WHITESPACE)
                (COMMENT)
                (STRING)
                ('(')(')')('+')('-')('*')('/')('%')('=')('>')('<')('&')('|')(';')(',')('.')('~')
                (EQ_OP)(NEQ_OP)(LE_OP)(GE_OP)(SHLEFT_OP)(SHRIGHT_OP)(CONCAT_OP)
                (UNKNOWN);
        }

        void_token_def      WHITESPACE;
        void_token_def      COMMENT;
        string_token_def    STRING;
        void_token_def      EQ_OP;
        void_token_def      NEQ_OP;
        void_token_def      LE_OP;
        void_token_def      GE_OP;
        void_token_def      SHLEFT_OP;
        void_token_def      SHRIGHT_OP;
        void_token_def      CONCAT_OP;
        string_token_def    BLOB;
        string_token_def    IDENTIFIER;
        string_token_def    BIND_PARAMETER;
        string_token_def    NUMERIC_LITERAL;
        string_token_def    INTEGER;
        void_token_def      UNKNOWN;

        // NULL is treated differently, since it causes havoc, being a C macro as well,
        // #undef-ing NULL fails on a g++ __null intrinsic.
        void_token_def      NULL_T;

        // boilerplate the other keywords.
        BOOST_PP_SEQ_FOR_EACH( DISSEQT_LEXER_DECLARE_MEMBER, void_token_def, DISSEQT_KEYWORDS)
    };

}


#endif /* LEXER_H_ */
