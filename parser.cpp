/*
 * parser.cpp
 *
 *  Created on: Apr 16, 2015
 *      Author: danny
 */
#include "lexer.h"
#include "disseqt_grammar.h"
#include <iostream>
#include <fstream>

namespace disseqt
{
namespace qi = boost::spirit::qi;
bool parse( std::string::const_iterator &first, std::string::const_iterator last)
{
    typedef std::string::const_iterator Iterator;
    typedef LexerTypes<Iterator>::base_lexer_type BaseLexer;
    typedef LexerTypes<Iterator>::iterator_type LexerIterator;

    Lexer<BaseLexer> l;
    SqlGrammar<LexerIterator> g(l);
    qi::rule<LexerIterator> skipper = l.COMMENT|l.WHITESPACE;

    try
    {
        return tokenize_and_phrase_parse( first, last, l, g, skipper, boost::spirit::qi::skip_flag::postskip);
    }
    catch ( qi::expectation_failure<LexerTypes<Iterator>::iterator_type> &e)
    {
        std::cerr << "***\n";
        std::cerr << "expectation failure:\n";
        auto it = e.first;
        auto count = 10;

        while (count-- && it != e.last)
        {
            std::cerr << it->value();
            ++it;
        }
        std::cerr << "\n";
        return false;
    }
}

void test( const std::string &filename)
{

    std::string buffer;
    std::ifstream inputfile( filename);

    size_t succeeded = 0;
    size_t failed = 0;
    while (getline( inputfile, buffer))
    {
        try
        {
            auto first = buffer.cbegin();
            if (!parse( first, buffer.cend()) || first != buffer.cend())
            {
                ++failed;
                std::cout << "**********\n";
                std::cout << buffer << '\n';
                std::cout << "**********\n";
                std::cout << std::string(first, buffer.cend())<< '\n';
                std::cout << "**********\n";
            }
            else
            {
                ++succeeded;
            }
        }
        catch ( qi::expectation_failure<std::string::const_iterator> &e)
        {
            ++failed;
            std::cerr << "***\n";
            std::cerr << "expectation failure:\n";
            std::cerr << std::string( buffer.cbegin(), e.first) << '\n';
            std::cerr << "<HERE>\n";
            std::cerr << std::string( e.first, e.last) << '\n';
        }
        catch (std::exception &e)
        {
            ++failed;
            std::cerr << "****\n";
            std::cerr << "Parsing failure: " << e.what() << '\n';
            std::cout << buffer << '\n';
        }
    }

    std::cout << succeeded << " queries succeeded, " << failed << " failed\n";
}

}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        disseqt::test( argv[1]);
    }
    return 0;
}

