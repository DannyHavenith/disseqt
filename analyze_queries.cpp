/**
 * @file
 *
 * Copyright (c) 2018 Mapscape B.V.
 *
 */

#include "disseqt/parser.h"
#include "disseqt/disseqt_visitor_builders.h"
#include "disseqt/disseqt_ast_print.h"

#include <boost/range/algorithm/set_algorithm.hpp>
#include <boost/range/adaptor/map.hpp>

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <map>

namespace {
using StringSet = std::set<std::string>;

// for every field, not the field name and the potential tables the field
// may be coming from.
using FieldInfo = std::map<std::string, StringSet>;
class NamesCollector
{
public:
    bool operator()( const disseqt::ast::composite_column_name &name)
    {
        const auto fieldName = name.column.to_string();
        auto &fieldInfo = m_fields[fieldName];
        (void)fieldInfo;
        // if a table name is mentioned, then it is clear that this
        // field can only be from this table.
        if (name.table)
        {
            fieldInfo = { name.table->to_string()};
        }
        return false;
    }

    bool operator()( const disseqt::ast::table_name &table)
    {
        m_tables.insert( table.to_string());
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

    FieldInfo GetFields() const
    {
        return m_fields;
    }

    StringSet GetAliases() const
    {
        return m_aliases;
    }

    StringSet GetTables() const
    {
        return m_tables;
    }

private:
    FieldInfo m_fields;
    StringSet m_aliases;
    StringSet m_tables;
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
                .everywhere()
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

        const auto nestedTables = nested.GetTables();
        m_tables.insert( nestedTables.begin(), nestedTables.end());

        // add any field reference that is not an alias to our field list
        const auto nestedAliases = nested.GetAliases();
        auto collectedFields = collected.GetFields();
        auto collectedTables = collected.GetTables();
        m_tables.insert( collectedTables.begin(), collectedTables.end());


        for (const auto &field : collectedFields)
        {
            if (not nestedAliases.count( field.first))
            {
                auto &destinationField = m_fields[field.first];
                if (destinationField.size() != 1)
                {
                    if (field.second.size() == 1)
                    {
                        destinationField = field.second;
                    }
                    else if (field.second.empty())
                    {
                        destinationField = m_tables;
                    }
                }
            }
        }

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

    FieldInfo GetFields() const
    {
        return m_fields;
    }

    StringSet GetTables() const
    {
        return m_tables;
    }

private:
    StringSet m_aliases;
    FieldInfo m_fields;
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

std::ostream &PrintStrings( std::ostream &output, const std::string &prefix, const FieldInfo &fields)
{
    for (const auto &field : fields)
    {
        output << prefix << ": " << field.first << " {";
        for (const auto &table : field.second)
        {
            output << ' ' << table ;
        }
        output << "}\n";
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

