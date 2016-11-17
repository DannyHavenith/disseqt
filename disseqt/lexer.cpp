/*
 * lexer.cpp
 *
 *  Created on: Apr 16, 2015
 *      Author: danny
 */

#include "lexer.h"
#include <boost/preprocessor/stringize.hpp>

#define DISSEQT_LEXER_INITIALIZER( r, data, keyword) , keyword( "(?i:" BOOST_PP_STRINGIZE(keyword) ")")
#define DISSEQT_LEXER_BARE_PREFIX( r, prefix, keyword)  prefix keyword

namespace disseqt
{
    template<typename L>
    Lexer<L>::Lexer()
        :
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
             ("DOUBLE_QUOTED",  R"('[^'\\]*(\\.[^'\\]*)*')")
             ("SINGLE_QUOTED",  R"(\"[^"\\]*(\\.[^"\\]*)*\")")
             ("IDENT",          R"([a-zA-Z_][a-zA-Z0-9_]*)")
             ("HEX",            R"(0[xX][0-9a-fA-F]+)")
             ("NUM",            R"((\d+(\.\d*)?|\.\d+)([eE]\d+)?)")
             ("WHITESPACE",     R"(\s+)")
             ("COMMENT",        R"(\/\*[^*]*\*+([^/*][^*]*\*+)*\/|\/\/.*$)")
             ;

         STRING = "{DOUBLE_QUOTED}|{SINGLE_QUOTED}";
         BLOB = "[Xx]({DOUBLE_QUOTED}|{SINGLE_QUOTED})";
         IDENTIFIER = "{IDENT}";
         BIND_PARAMETER = "\\?\\d*|:{IDENT}|@{IDENT}|\\${IDENT}(::{IDENT}?)*(\\([^)]*\\))?";
         NUMERIC_LITERAL = "{HEX}|{NUM}";
         WHITESPACE = "{WHITESPACE}";
         COMMENT = "{COMMENT}";
         SKIPPER = "{WHITESPACE}|{COMMENT}";

         this->self.add
             DISSEQT_KEYWORDS
             (NULL_T)
             (BLOB)
             (IDENTIFIER)
             (BIND_PARAMETER)
             (NUMERIC_LITERAL)
             (WHITESPACE)
             (COMMENT)
             (SKIPPER)
             (STRING)
             ('(')(')')('+')('-')('*')('/')('%')('=')('>')('<')('&')('|')(';')('.')(',')('~')
             (EQ_OP)(NEQ_OP)(LE_OP)(GE_OP)(SHLEFT_OP)(SHRIGHT_OP)(CONCAT_OP)
             (UNKNOWN);
     }

    // make sure a lexer for std::string::const_iterator is instantiated in this translation unit.
    typedef LexerTypes<std::string::const_iterator> InstantiatedLexerTypes;
    template class Lexer<InstantiatedLexerTypes::base_lexer_type>;
}





