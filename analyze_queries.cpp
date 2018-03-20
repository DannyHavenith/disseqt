/**
 * @file
 *
 * Copyright (c) 2018 Mapscape B.V.
 *
 */

#include "disseqt/parser.h"
#include "disseqt/disseqt_visitor_builders.h"

#include <boost/range/algorithm/set_algorithm.hpp>

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>

namespace {
using StringSet = std::set<std::string>;
class NamesCollector
{
public:
    bool operator()( const disseqt::ast::column_name &name)
    {
        if (not name.to_string().empty())
        {
            m_names.insert( name.to_string());
        }
        return false;
    }

    bool operator()( const disseqt::ast::column_alias& alias)
    {
        if (not alias.to_string().empty())
        {
            m_aliases.insert( alias.to_string());
        }
        return false;
    }

    /**
     * Don't recurse into select statements,
     */
    bool operator()( const disseqt::ast::select_statement &)
    {
        return false;
    }

    StringSet GetNames() const
    {
        return m_names;
    }

    StringSet GetAliases() const
    {
        return m_aliases;
    }

private:
    StringSet m_names;
    StringSet m_aliases;
};
}

class SelectAnalyzer
{
public:
    bool operator()( const disseqt::ast::select_phrase &phrase)
    {
        using namespace disseqt::ast;
        using namespace disseqt;

        // collect every field name that is directly used in this select statement
        // and every alias that is introduced by this select statement.
        // Some of the field names might be aliases introduced by embedded select statements.
        auto collected =
                apply<NamesCollector>()
                .in_every<column_name, column_alias, select_statement>()
                .in( phrase);

        // add the aliases to our alias list.
        const auto collectedAliases = collected.GetAliases();
        m_aliases.insert( collectedAliases.begin(), collectedAliases.end());


        // now hunt for embedded select statements and make sure that
        // all aliases from them get striked from the field list.
        // also add all aliases from embedded selects to the alias list.
        auto nested =
                apply<SelectAnalyzer>()
                .everywhere()
                .in( phrase.from, phrase.where, phrase.having);

        // add any field reference that is not an alias to our field list
        const auto nestedAliases = nested.GetAliases();
        auto collectedFields = collected.GetNames();
        boost::range::set_difference( collectedFields, nestedAliases, std::inserter( m_fields, m_fields.end()));

        // add aliases of nested queries to the alias list.
        m_aliases.insert( nestedAliases.begin(), nestedAliases.end());

        // add field references of nested queries to the alias list.
        const auto nestedFields = nested.GetFields();
        m_fields.insert( nestedFields.begin(), nestedFields.end());


        return false;
    }

    StringSet GetAliases() const
    {
        return m_aliases;
    }

    StringSet GetFields() const
    {
        return m_fields;
    }

private:
    StringSet m_aliases;
    StringSet m_fields;
    StringSet m_tables;
};

std::ostream &PrintStrings( std::ostream &output, const std::string &prefix, const StringSet &strings)
{
    for (const auto &string : strings)
    {
        output << prefix << ": " << string << '\n';
    }

    return output;
}

std::string GetFileContent( const std::string &filename)
{
    std::ifstream     file( filename);
    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

int main( int argc, char *argv[])
{
    using namespace disseqt;
    using namespace disseqt::ast;

    try
    {
        const std::string teststring = GetFileContent( argv[1]);

        std::cout << "query: " << teststring << '\n';
        const auto ast = parse( teststring);

        const auto aliasCollector =
                disseqt::apply<SelectAnalyzer>()
                .everywhere()
                .in( ast);

        PrintStrings( std::cout, "alias", aliasCollector.GetAliases());
        PrintStrings( std::cout, "name",  aliasCollector.GetFields());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Something went wrong: " << e.what() << '\n';
    }

    return 0;
}

