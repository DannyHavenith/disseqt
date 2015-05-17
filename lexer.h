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
#include <boost/spirit/include/lex_lexertl.hpp>

#define DISSEQT_LEXER_DECLARE_MEMBER( r, type, keyword) type keyword;

namespace disseqt {

    namespace lex = boost::spirit::lex;

    template< typename Iterator>
    struct LexerTypes
    {
        // I want a token that holds an iterator pair
        typedef lex::lexertl::token< Iterator>              token_type;
        typedef lex::lexertl::lexer< token_type>            base_lexer_type;
        typedef typename base_lexer_type::iterator_type     iterator_type;
    };

    typedef std::string::const_iterator PositionIterator;

    template< typename L>
    struct Lexer : boost::spirit::lex::lexer<L>
    {
        typedef boost::spirit::lex::token_def<boost::spirit::lex::omit> void_token_def;
        typedef boost::spirit::lex::token_def<boost::iterator_range<PositionIterator>> string_token_def;

        Lexer();

        void_token_def      WHITESPACE;
        void_token_def      COMMENT;
        void_token_def      SKIPPER;
        void_token_def      EQ_OP;
        void_token_def      NEQ_OP;
        void_token_def      LE_OP;
        void_token_def      GE_OP;
        void_token_def      SHLEFT_OP;
        void_token_def      SHRIGHT_OP;
        void_token_def      CONCAT_OP;
        void_token_def      UNKNOWN;
        void_token_def      PERIOD;

        string_token_def    STRING;
        string_token_def    BLOB;
        string_token_def    IDENTIFIER;
        string_token_def    BIND_PARAMETER;
        string_token_def    NUMERIC_LITERAL;
        string_token_def    INTEGER;

        // NULL is treated differently, since it causes havoc, being a C macro as well,
        // #undef-ing NULL fails on a g++ __null intrinsic.
        void_token_def      NULL_T;

        // boilerplate the other keywords.
        BOOST_PP_SEQ_FOR_EACH( DISSEQT_LEXER_DECLARE_MEMBER, void_token_def, DISSEQT_SPECIFIC_KEYWORDS)
        BOOST_PP_SEQ_FOR_EACH( DISSEQT_LEXER_DECLARE_MEMBER, string_token_def, DISSEQT_NONSPECIFIC_KEYWORDS)
    };

}


#endif /* LEXER_H_ */
