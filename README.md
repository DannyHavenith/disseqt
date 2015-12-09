[![Build Status](https://travis-ci.org/DannyHavenith/disseqt.svg)](https://travis-ci.org/DannyHavenith/disseqt) (well at least at travis' ancient g++ 4.6 compiler it is)
Disseqt, a SQL(ite) parser
=====

This is a Work In Progress SQL parser written using [boost.spirit](www.boost.org/doc/libs/release/libs/spirit/doc/html/index.html).


The parser generates a strongly typed Abstract Syntax Tree that can be traversed with several types of visitors. Visitors can also be constructed with an embedded language, best demonstrated with a code example:

```C++

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
```

(see [the unit test sources](test/queries.cpp))




 
