/*
 * parser.cpp
 *
 *  Created on: Apr 16, 2015
 *      Author: danny
 */
#include "lexer.h"
#include "disseqt_grammar.h"
#include "disseqt_ast_statements.h"
#include <iostream>
#include <fstream>

#include "disseqt_ast_print.h"
#include "disseqt_visitor.h"

namespace disseqt
{
namespace qi = boost::spirit::qi;

    class TableNameCollector : public AstVisitor< TableNameCollector>
    {
    public:

        /// ignore anything that is not a table name.
        template< typename T>
        bool Visit( const T&value)
        {
            return true;
        }

        bool Visit( const ast::composite_table_name &table)
        {
            const std::string dbname = table.database?table.database->to_string():"";
            m_names.push_back( std::make_pair( dbname, table.table.to_string() ));
            return false;
        }

        typedef std::vector< std::pair<std::string, std::string>> Names;
        Names GetNames() const
        {
            return m_names;
        }

    private:
        Names m_names;
    };

/**
 * Parse a SQL query and report whether the parse succeeded.
 */
bool parse( std::string::const_iterator &first, std::string::const_iterator last, ast::sql_stmt_list &statements)
{
    typedef std::string::const_iterator Iterator;
    typedef LexerTypes<Iterator>::base_lexer_type BaseLexer;
    typedef LexerTypes<Iterator>::iterator_type LexerIterator;

    Lexer<BaseLexer> l;
    SqlGrammar<LexerIterator> g(l);
    qi::rule<LexerIterator> skipper = l.COMMENT|l.WHITESPACE;

    try
    {
        return tokenize_and_phrase_parse(
                first, last, l, g,
                skipper,
                boost::spirit::qi::skip_flag::postskip,
                statements);
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

/**
 * Read a bunch of newline-separated SQL queries from a file and run them to through the parser.
 */
void test( const std::string &filename)
{
    std::string buffer;
    std::ifstream inputfile( filename);

    size_t succeeded = 0;
    size_t failed = 0;
    ast::sql_stmt_list statements;

    if (getline( inputfile, buffer))
    {
        try
        {
            auto first = buffer.cbegin();
            if (!parse( first, buffer.cend(), statements) || first != buffer.cend())
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
                if (!statements.empty())
                {
                    // print the table names used in the query
                    TableNameCollector vis;
                    vis( statements[0]);

                    std::cout << "table names:\n";
                    for (const auto &name : vis.GetNames())
                    {
                        if (not name.first.empty())
                        {
                            std::cout << name.first << '.';
                        }
                        std::cout << name.second << '\n';
                    }
                }
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
    else
    {
        std::cerr << "no arguments provided\n";
    }
    return 0;
}

