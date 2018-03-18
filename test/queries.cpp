/*
 * queries.cpp
 *
 *  Created on: Dec 8, 2015
 *      Author: danny
 */
#include "disseqt_visitor_builders.h"
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
        return true;
    }

    StringVector GetNames() const
    {
        return m_names;
    }

    void Add( const std::string &value)
    {
        m_names.push_back( value);
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
        WHERE source1.field3 = source2.field4
        )";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( insertAndSelect));

    auto collectNames =
            disseqt::apply<NamesCollector>()
            .in_every<column_name>()
            .in( ast);

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
        WHERE source1.field3 = source2.field4
        )";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( insertAndSelect));

    auto collectNames =
        apply<NamesCollector>()
        .in_every<column_name>()
        .within<result_column>()
        .in( ast);

    EXPECT_EQ( (StringVector{ "field1", "field2"}), collectNames.GetNames());
}

TEST(AstQueries, NestedWithin)
{
    const std::string insertAndSelect = R"(
        INSERT INTO destination(d1,d2) 
        SELECT field1, field2 
        FROM source1, (SELECT nested1, nested2 + nested3 FROM nested_source WHERE nested3 = 1) 
        WHERE source1.field3 = source2.field4
        )";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( insertAndSelect));

    auto columnNames =
        apply<NamesCollector>()
        .in_every<column_name>()
        .within<result_column>()       // result columns are what follows a "SELECT"
        .within<join_clause>()         // join clause is what follows a "FROM"
        .in( ast);

    EXPECT_EQ( (StringVector{"nested1", "nested2", "nested3"}), columnNames.GetNames());

    auto tableNames =
        apply<NamesCollector>()
        .in_every<table_name>()
        .within<select_statement>()
        .within<join_clause>()         // join clause is what follows a "FROM"
        .in( ast);

    EXPECT_EQ( (StringVector{"nested_source"}), tableNames.GetNames());

}

TEST(AstQueries, MemberSelection)
{
    const std::string insertAndSelect = R"(
        INSERT INTO destination(d1,d2) 
        SELECT field1, field2 
        FROM source1, (SELECT nested1, nested2 + nested3 FROM nested_source WHERE nested4 = 1) 
        WHERE source1.field3 = source2.field4
        )";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( insertAndSelect));

    auto columnNames =
        apply<NamesCollector>()
        .in_every<column_name>()
        .within( &select_phrase::columns)
        .within( &select_phrase::from)
        .in( ast);

    EXPECT_EQ( (StringVector{"nested1", "nested2", "nested3"}), columnNames.GetNames());
}

TEST(AstQueries, InitializedVisitor)
{
    const std::string insertAndSelect = R"(
        INSERT INTO destination(d1,d2) 
        SELECT field1, field2 
        FROM source1, (SELECT nested1, nested2 + nested3 FROM nested_source WHERE nested3 = 1) 
        WHERE source1.field3 = source2.field4
        )";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( insertAndSelect));

    NamesCollector collector;
    collector.Add( "initial");

    auto columnNames =
        apply( collector)
        .in_every<column_name>()
        .within( &select_phrase::columns)
        .within( &select_phrase::from)
        .in( ast);

    EXPECT_EQ( (StringVector{"initial","nested1", "nested2", "nested3"}), columnNames.GetNames());

}


TEST( AstQueries, CombinedVisitors)
{
    const std::string insertAndSelect = R"(
        INSERT INTO destination(d1,d2)
        SELECT field1, field2
        FROM source1, (SELECT nested1, nested2 + nested3 FROM nested_source1, nested_source2 WHERE nested3 = 1) 
        WHERE source1.field3 = source2.field4
        )";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( insertAndSelect));

    // visitor that finds table names in the from-part of selects inside
    // insert values or with-clauses.
    auto sourceTablesCollector =
            apply<NamesCollector>()
            .in_every<table_name>()
            .within<>(&select_phrase::from)
            .within<insert_values, with_clause>();

    // visitor that finds table names in the 'into'-part of an
    // insert statement
    auto destinationTablesCollector =
            apply<NamesCollector>()
            .in_every<table_name>()
            .within( &insert_stmt::table);

    StringVector sourceNames;
    StringVector destinationNames;

    // create a complex visitor that will search for insert statements and if
    // it finds one, will release the two previously declared visitors into that
    // statement.
    apply([&sourceTablesCollector,
           &destinationTablesCollector,
           &sourceNames,
           &destinationNames]
           ( const insert_stmt &insert)
           {
                sourceNames
                    = sourceTablesCollector.in( insert).GetNames();
                destinationNames
                    = destinationTablesCollector.in(insert).GetNames();

                return false;
           })
    .everywhere()
    .in( ast);

    EXPECT_EQ( (StringVector{"destination"}), destinationNames);
    EXPECT_EQ( (StringVector{"source1", "nested_source1", "nested_source2"}), sourceNames);


}

TEST( AstQueries, BindingParameters)
{
    const std::string nestedWithParameters = R"(
        SELECT field1, :par1, ?42, $par3::no::idea::why( I would want this)
        FROM table1 JOIN table2 ON table2.field2 == :par4, (SELECT :par5 FROM table2)
        WHERE :par6 == :par7 AND par8 <> a_parameter AND :par9 AND :par10
)";

    ast::sql_stmt_list ast;
    ASSERT_NO_THROW( ast = disseqt::parse( nestedWithParameters));

    auto parameterNames =
        apply<NamesCollector>()
        .in_every<parameter_name>()
        .in( ast);

    EXPECT_EQ(
            (StringVector{ ":par1", "?42", "$par3::no::idea::why( I would want this)", ":par4", ":par5", ":par6", ":par7", ":par9", ":par10"}),
            parameterNames.GetNames());

}
