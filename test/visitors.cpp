#include "parser.h"
#include "disseqt_visitor.h"
#include <gtest/gtest.h>
#include <utility>
#include <vector>

namespace
{
    class TableNameCollector
    {
    public:

        /// ignore anything that is not a table name.
        template< typename T>
        bool operator()( const T&value)
        {
            return true;
        }

        bool operator()( const disseqt::ast::composite_table_name &table)
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
}

TEST( Visitors, TypeSelect)
{
    const std::string q = "select a, b, c from x, databasex.y, z where a == 1";
    auto syntaxTrees = disseqt::parse(q);

    ASSERT_EQ( 1, syntaxTrees.size());

    TableNameCollector visitor;
    disseqt::VisitTopDown( visitor, syntaxTrees);
    auto names = visitor.GetNames();

    ASSERT_EQ( 3, names.size());

    EXPECT_EQ( "x", names[0].second);
    EXPECT_EQ( "y", names[1].second);
    EXPECT_EQ( "z", names[2].second);

    EXPECT_TRUE( names[0].first.empty());
    EXPECT_EQ( "databasex", names[1].first);
    EXPECT_TRUE( names[2].first.empty());
}

