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


#define DISSEQT_LEXER_INITIALIZER( r, data, keyword) , keyword(BOOST_PP_STRINGIZE(keyword))
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
         STRING(R"('[^'\\]*(\\.[^'\\]*)*'|\"[^"\\]*(\\.[^"\\]*)*\")"),
         EQ_OP("=="),
         NEQ_OP("!=|<>"),
         LE_OP("<="),
         GE_OP(">="),
         SHLEFT_OP("<<"),
         SHRIGHT_OP(">>"),
         CONCAT_OP("\\|\\|"),
         UNKNOWN("."),
         NULL_T("NULL")
         BOOST_PP_SEQ_FOR_EACH( DISSEQT_LEXER_INITIALIZER, _, DISSEQT_KEYWORDS)
        {
            // TODO: identifier, numeric literals, blob literal, bind_parameter
            this->self.add
                DISSEQT_KEYWORDS
                (NULL_T)
                (IDENTIFIER)
                (WHITESPACE)
                (COMMENT)
                (STRING)
                ('(')(')')('+')('-')('*')('/')('%')('=')('>')('<')('&')('|')('?')(';')(',')('.')('~')
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
        void_token_def      UNKNOWN;
        void_token_def      NULL_T; // NULL is treated differently, since it causes havoc, being a C macro as well
        BOOST_PP_SEQ_FOR_EACH( DISSEQT_LEXER_DECLARE_MEMBER, void_token_def, DISSEQT_KEYWORDS)
    };

}


#endif /* LEXER_H_ */
