/*
 * parser.cpp
 *
 *  Created on: Apr 16, 2015
 *      Author: danny
 */

#include "parser.h"
#include "lexer.h"
#include "disseqt_grammar.h"
#include "disseqt_ast_statements.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>



namespace disseqt
{
namespace qi = boost::spirit::qi;


/**
 * Parse a SQL query and return the resulting ast list.
 *
 * The first iterator will be incremented one beyond the last parsed token. If parsing
 * succeeds partially, this iterator will not be equal to the end-iterator, but no error
 * will be thrown.
 *
 * If parsing fails due to a syntax error within a statement, this function will throw a parser_error.
 */
ast::sql_stmt_list parse( std::string::const_iterator &first, std::string::const_iterator last)
{
    typedef std::string::const_iterator Iterator;
    typedef LexerTypes<Iterator>::base_lexer_type BaseLexer;
    typedef LexerTypes<Iterator>::iterator_type LexerIterator;

    Lexer<BaseLexer> l;
    SqlGrammar<LexerIterator> g(l);
    qi::rule<LexerIterator> skipper = l.COMMENT|l.WHITESPACE;
    ast::sql_stmt_list statements;
    try
    {
        tokenize_and_phrase_parse(
                first, last, l, g,
                skipper,
                boost::spirit::qi::skip_flag::postskip,
                statements);
    }
    catch ( qi::expectation_failure<LexerTypes<Iterator>::iterator_type> &e)
    {
        std::ostringstream errormessage;
        errormessage << "***\n";
        errormessage << "expectation failure:\n";
        auto it = e.first;
        auto count = 10;

        while (count-- && it != e.last)
        {
            errormessage << it->value();
            ++it;
        }
        errormessage << "\n";
        throw parser_error( errormessage.str());
    }

    return statements;
}

/**
 * Parse a string containing sql statements and return an AST list.
 *
 * If the string can't be parsed completely, this function will thrown a
 * parser_error.
 */
ast::sql_stmt_list parse( const std::string &sql)
{
    auto first = sql.begin();
    auto result = parse( first, sql.end());

    if (first != sql.end())
    {
        std::stringstream errormessage;
        errormessage << "Incomplete parse, remaining:\n";
        errormessage << "**********\n";
        errormessage << std::string(first, sql.end())<< '\n';
        errormessage << "**********\n";
        throw parser_error( errormessage.str());
    }

    return result;
}

bool is_valid( const std::string &sql)
{
    bool result = true;
    try
    {
        parse( sql);
    }
    catch( const parser_error &)
    {
        result = false;
    }

    return result;
}

/**
 * Parse sql statements from an input stream.
 *
 * This function will first read the complete input stream into an internal buffer
 * and then parse it.
 */
ast::sql_stmt_list parse( std::istream &inputstream)
{
    const std::string buffer{
            std::istreambuf_iterator<char>{ inputstream},
            std::istreambuf_iterator<char>{} };

    return parse( buffer);
}
//
///**
// * Read a bunch of newline-separated SQL queries from a file and run them through the parser.
// */
//void test( const std::string &filename)
//{
//    std::string buffer;
//    std::ifstream inputfile( filename);
//
//    size_t succeeded = 0;
//    size_t failed = 0;
//    ast::sql_stmt_list statements;
//
//    if (getline( inputfile, buffer))
//    {
//        try
//        {
//            auto first = buffer.cbegin();
//            if (!parse( first, buffer.cend(), statements) || first != buffer.cend())
//            {
//                ++failed;
//                std::cout << "**********\n";
//                std::cout << buffer << '\n';
//                std::cout << "**********\n";
//                std::cout << std::string(first, buffer.cend())<< '\n';
//                std::cout << "**********\n";
//            }
//            else
//            {
//                ++succeeded;
//                if (!statements.empty())
//                {
//                    // print the table names used in the query
//                    TableNameCollector vis;
//                    VisitTopDown( vis, statements[0]);
//
//                    std::cout << "table names:\n";
//                    for (const auto &name : vis.GetNames())
//                    {
//                        if (not name.first.empty())
//                        {
//                            std::cout << name.first << '.';
//                        }
//                        std::cout << name.second << '\n';
//                    }
//                }
//            }
//        }
//        catch ( qi::expectation_failure<std::string::const_iterator> &e)
//        {
//            ++failed;
//            std::cerr << "***\n";
//            std::cerr << "expectation failure:\n";
//            std::cerr << std::string( buffer.cbegin(), e.first) << '\n';
//            std::cerr << "<HERE>\n";
//            std::cerr << std::string( e.first, e.last) << '\n';
//        }
//        catch (std::exception &e)
//        {
//            ++failed;
//            std::cerr << "****\n";
//            std::cerr << "Parsing failure: " << e.what() << '\n';
//            std::cout << buffer << '\n';
//        }
//    }
//
//    std::cout << succeeded << " queries succeeded, " << failed << " failed\n";
//}
//
}

