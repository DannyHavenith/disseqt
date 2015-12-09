/*
 * queries.cpp
 *
 *  Created on: Dec 8, 2015
 *      Author: danny
 */
#include "disseqt_ast_queries.h"
#include "parser.h"
#include <gtest/gtest.h>

namespace {

using StringVector = std::vector<std::string>;

class NamesCollector
{
public:
    template< typename NameType>
    bool operator()( const NameType &name)
    {
        m_names.push_back( name.to_string());
    }

    StringVector GetNames() const
    {
        return m_names;
    }

private:
    StringVector m_names;
};

}

using namespace disseqt::ast;
using namespace disseqt;

TEST( AstQueries, ForEvery)
{

    const std::string insertAndSelect = R"(
        INSERT INTO destination(d1,d2) 
        SELECT field1, field2 
        FROM source1, source2 
        WHERE source1.field3 = source2.field4)";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( insertAndSelect));

    auto collectNames =
            for_every<column_name>
            ::apply<NamesCollector>( ast);

    EXPECT_EQ(
            (StringVector{"d1","d2","field1", "field2","field3","field4"}),
            collectNames.GetNames()
            );
}

TEST( AstQueries, Within)
{
    const std::string insertAndSelect = R"(
        INSERT INTO destination(d1,d2) 
        SELECT field1, field2 
        FROM source1, source2 
        WHERE source1.field3 = source2.field4)";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( insertAndSelect));

    auto collectNames =
        for_every< column_name>
        ::within< result_column>
        ::apply<NamesCollector>( ast);

    EXPECT_EQ( (StringVector{ "field1", "field2"}), collectNames.GetNames());
}

TEST(AstQueries, NestedWithin)
{
    const std::string insertAndSelect = R"(
        INSERT INTO destination(d1,d2) 
        SELECT field1, field2 
        FROM source1, (SELECT nested1, nested2 FROM nested_source WHERE nested3 = 1) 
        WHERE source1.field3 = source2.field4)";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( insertAndSelect));

    auto collectNames =
        for_every<column_name>
        ::within<result_column>       // result columns are what follows a "SELECT"
        ::within<join_clause>         // join clause is what follows a "FROM"
        ::apply<NamesCollector>( ast);

    EXPECT_EQ( (StringVector{"nested1", "nested2"}), collectNames.GetNames());

}


