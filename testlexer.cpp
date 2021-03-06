/*
 * testlexer.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: danny
 */
#include "lexer.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_container.hpp>

using namespace boost::spirit;
using namespace boost::spirit::ascii;

template <typename Iterator>
struct test_grammar : qi::grammar<Iterator>
{
    template <typename TokenDef>
    test_grammar(TokenDef const& tok)
      : test_grammar::base_type(start)
      , c(0), w(0), l(0)
    {
        using boost::phoenix::ref;
        using boost::phoenix::size;

        start =  *(     tok.SELECT        [ref(c) += size(_1)]
                  |     tok.WHITESPACE    [++ref(w)]
                  |     '('
                  |     tok.STRING        [ std::cout << _1 << '\n']
                  )
              ;

    }

    std::size_t c, w, l;
    qi::rule<Iterator> start;
};


int test( int argc, char *argv[])
{
    std::string test{"SELecT != ?123:abc$efg::hij::kl(sfio\")@mno     ()*^/* commenting/* * ***/'this is a \"string'.'and this\" is another string' \"plus yet 'another\\\" string\""};

    using namespace disseqt;

    typedef std::string::const_iterator Iterator;
    typedef LexerTypes<Iterator>::base_lexer_type BaseLexer;
    typedef LexerTypes<Iterator>::iterator_type LexerIterator;

    Lexer<BaseLexer> l;
    test_grammar<LexerIterator> g( l);

    auto first = test.cbegin();
    auto last = test.cend();

    bool r = tokenize_and_parse( first, last, l, g);
    if (r)
    {
        std::cout << "selects: " << g.w << "\n";
        std::cout << "not-equals: " << g.l << "\n";
        std::cout << "size of selects: " << g.c << "\n";
    }
    else
    {
        std::cerr << "parsing stopped at: " <<
                std::string( first, last) << '\n';
    }

    return 0;
}


