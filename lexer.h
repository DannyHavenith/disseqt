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
        Lexer()
        :WHITESPACE("\\s+"),
         COMMENT(R"(\/\*[^*]*\*+([^/*][^*]*\*+)*\/|\/\/.*$)"),
         STRING(R"('[^'\\]*(\\.[^'\\]*)*')")
         BOOST_PP_SEQ_FOR_EACH( DISSEQT_LEXER_INITIALIZER, _, DISSEQT_KEYWORDS)
        {
            this->self.add
                DISSEQT_KEYWORDS
                (WHITESPACE)
                (COMMENT)
                (STRING)
                ('$')('(')(')')('?')('<');
        }

        void_token_def WHITESPACE;
        void_token_def COMMENT;
        boost::spirit::lex::token_def<std::string> STRING;
        BOOST_PP_SEQ_FOR_EACH( DISSEQT_LEXER_DECLARE_MEMBER, void_token_def, DISSEQT_KEYWORDS)

    };

}


#endif /* LEXER_H_ */
