/*
 * File:   disseqt.cpp
 * Author: danny
 *
 * Created on April 4, 2015, 11:31 PM
 */
#include <boost/spirit/include/qi.hpp>
#include <string>
#include <iostream>
#include <fstream>

using namespace boost::spirit;
using namespace boost::spirit::ascii;

#undef NULL
#define KEYWORD( kw_) kw_ = no_case[lit(#kw_)]
#define DECL_KEYWORD( kw_ ) rule kw_

template< typename Iterator, typename Skipper>
struct SqlGrammar : qi::grammar<Iterator, Skipper>
{
    SqlGrammar()
    :SqlGrammar::base_type( sql_stmt)
    {
        sql_stmt_list
            =   sql_stmt % ';'
            ;


        sql_stmt =
                explain_stmt
            |   stmt
            ;

        explain_stmt =
                EXPLAIN > -( QUERY >> PLAN) >> stmt
            ;

        stmt = /* TODO: finish */
                update_stmt
            |   select_stmt
            |   create_table_stmt
            ;

        create_table_stmt =
                CREATE >> -(TEMP|TEMPORARY) >> TABLE
            >   -(IF > NOT > EXISTS)
            >>  composite_table_name
            >>  (
                    (AS > select_stmt)
                |   ('(' >> column_def%',' >> -(table_constraint%',') > ')' >> -( WITHOUT > ROWID))
                )
            ;

        column_def =
                column_name >> -type_name >> *column_constraint
            ;

        type_name =
                *identifier >> -( '(' > qi::int_ >> -(',' >> int_ ) > ')'   )
            ;

        column_constraint =
                -(CONSTRAINT > name)
            >>  (
                    (PRIMARY > KEY > -(ASC|DESC) >> -conflict_clause >> -AUTOINCREMENT)
                |   (NOT > NULL > -conflict_clause)
                |   (UNIQUE > -conflict_clause)
                |   (CHECK > '(' > expr > ')')
                |   (DEFAULT > (qi::int_|literal_value| ('(' > expr > ')')))
                |   (COLLATE > collation_name)
                |   foreign_key_clause
                )
            ;

        literal_value =
                numeric_literal
            |   string_literal
            |   blob_literal
            |   NULL
            |   CURRENT_TIME
            |   CURRENT_DATE
            |   CURRENT_TIMESTAMP
            ;

        // in the syntax spec for SQLite, this one has an empty alternative, this has been
        // removed and the conflict clause itself has become optional in all rules that use it.
        conflict_clause =
                ON > CONFLICT > (ROLLBACK|ABORT|FAIL|IGNORE|REPLACE)
            ;

        foreign_key_clause =
                REFERENCES >> foreign_table >> -( '(' > column_name%',' > ')')
            >>  *(
                    (ON > (DELETE|UPDATE) > (SET >> NULL | SET >> DEFAULT | CASCADE | RESTRICT | NO >> ACTION))
                |   (MATCH >> name)
                )
            >>  -(-NOT >> DEFERRABLE >> -(INITIALLY > (DEFERRED|IMMEDIATE)))
            ;

        table_constraint =
                -(CONSTRAINT > name)
            >>  (
                    ((PRIMARY >> KEY|UNIQUE) > '(' >> indexed_column%',' >> ')' >> -conflict_clause)
                |   (CHECK > '(' > expr > ')')
                |   (FOREIGN > KEY > '(' > column_name%',' > ')' > foreign_key_clause)
                )
            ;

        indexed_column =
                column_name >> -(COLLATE > collation_name) >> -(ASC|DESC)
            ;

        numeric_literal =
                (+digit || '.' >> *digit) >> -( 'E' > -(char_('+')|'-') > +digit)
            |   "0x" >> +xdigit;
            ;

        string_literal =
                '\'' >> lexeme[char_ - '\''] >> '\''
            ;

        blob_literal =
                no_case['x'] >> string_literal
            ;

        select_stmt =
                -with_clause
            >>  ( select_phrase | values_clause) % compound_operator
            >> -order_by_clause
            >> -limit_clause
           ;

        order_by_clause =
                ORDER >> BY >> ordering_term % ','
            ;

        limit_clause =
                LIMIT >> expr >> -((OFFSET|',') >> expr)
            ;


        select_phrase =
                SELECT > -(DISTINCT|ALL) > result_column%','
            >>  -(FROM > (join_clause| table_or_subquery%','))
            >>  -(WHERE > expr)
            >>  -(GROUP > BY > expr % ',' > -(HAVING > expr))
            ;

        result_column =
                '*'
            |   table_name >> '.' >> '*'
            |   expr >> -( -AS >> column_alias)
            ;

        values_clause =
                VALUES > ('(' >> expr % ',' >> ')') % ','
            ;

        table_or_subquery =
                table_clause
            |   '(' >> (table_or_subquery%',' | join_clause) >> ')'
            |   '(' >> select_stmt >> ')' >> -( -AS >> table_alias)
            ;

        table_clause =
                composite_table_name
            >>  -( -AS >> table_alias)
            >>  -index_clause
            ;


        composite_table_name =
                -(database_name >> '.') >> table_name
            ;

        index_clause =
                NOT >> INDEXED | INDEXED >> BY >> index_name
            ;

        join_clause =
                table_or_subquery >> *(join_operator >> table_or_subquery >> -join_constraint)
            ;

        join_operator =
                ','
            |   -NATURAL >> -( LEFT >> -OUTER | INNER | CROSS) >> JOIN
            ;

        // officially contains an empty (epsilon) alternative, but instead we're making this
        // rule optional wherever it is used.
        join_constraint =
                (ON > expr)
            |   (USING > '(' > column_name%',' > ')')
            ;

        with_clause =
                WITH > -RECURSIVE >> common_table_expression %','
            ;

        common_table_expression =
                table_name >> -( '(' >> column_name%',' >> ')')
            >>  AS >> '(' >> select_stmt >> ')';
            ;

        compound_operator =
                UNION >> -ALL
            |   INTERSECT
            |   EXCEPT
            ;

        ordering_term =
                expr >> -(COLLATE > collation_name) >> -(ASC | DESC)
            ;

        update_stmt =
                -with_clause >> UPDATE >> -weasel_clause >> qualified_table_name
            >>  SET >> (column_name >> '=' > expr)%',' >> (WHERE > expr)
            >>  -update_limited_clause
            ;

        qualified_table_name =
                composite_table_name >> -index_clause
            ;

        update_limited_clause =
                -order_by_clause >> limit_clause
            ;

        weasel_clause =
                OR >
                (
                    ROLLBACK
                |   ABORT
                |   REPLACE
                |   FAIL
                |   IGNORE
                )
            ;

         // TODO: finish, this is just a quick implementation of expressions
        expr =
                or_oper >> *( OR > or_oper)
            ;

        or_oper =
                and_oper >> *( AND > and_oper)
            ;

        and_oper =
                compare_oper >> *(comparison_operator >> compare_oper)
            ;

        comparison_operator =
                lit("==") | "=" | "!=" |  "<>" | IS >> NOT | IS | IN | LIKE | GLOB | MATCH | REGEXP
            ;

        compare_oper =
                ineq_oper >> *( ineq_operator >> ineq_oper)
            ;

        ineq_operator =
                lit("<=") | '<' | ">=" | '>'
            ;

        ineq_oper =
                bitwise_oper >> *(bitwise_operator >> bitwise_oper)
            ;

        bitwise_operator =
                lit("<<") | ">>" | '&' | '|'
            ;

        bitwise_oper =
                term >>  *('+' >> term |'-' >> term)
            ;

        term =
               factor >> *( '*' >> factor | '/' >> factor)
            ;

        factor =
                literal_value
            |   bind_parameter
            |   (function_name >> '(' > -( '*'| -DISTINCT >> expr%','))
            |   composite_column_name
            |   '(' >> expr >> ')'
            |   ('-' >> factor)
            |   ('+' >> factor)
            ;

        // todo: make more efficient
        composite_column_name =
                database_name >> '.' >> table_name >> '.' >> column_name
            |   table_name >> '.' >> column_name
            |   column_name
            ;

        bind_parameter =
                lexeme[ char_('?') >> *digit]
            |   lexeme[ char_(':') >> (alpha|char_('_')) >> *(alnum|char_('_'))]
            |   lexeme[ char_('$') >> ((alpha|char_('_')) >> *(alnum|char_('_')))%"::" >> -( '(' >> *(char_ - ')') >> ')')]
            ;

        function_name = identifier.alias()
            ;
        foreign_table = name.alias()
            ;
        index_name = name.alias()
            ;
        table_name = name.alias()
            ;
        database_name = name.alias()
            ;
        collation_name = name.alias()
            ;
        column_name = name.alias()
            ;
        table_alias = name.alias()
            ;
        column_alias = name.alias()
            ;

        identifier =
                (alpha|char_('_')) >> *(alnum|char_('_'))
            ;

        tcl_identifier =
                (alpha|char_('_')) >> *(alnum|char_('_'))%"::" >> -( '(' >> *(char_ - ')') >> ')')
            ;

        // names can be bare identifiers, but also be quoted.
        name =
                identifier.alias()
            ;


        KEYWORD(IS);
        KEYWORD(IN);
        KEYWORD(LIKE);
        KEYWORD(GLOB);
        KEYWORD(REGEXP);
        KEYWORD(FOREIGN);
        KEYWORD(ON);
        KEYWORD(USING);
        KEYWORD(NATURAL);
        KEYWORD(LEFT);
        KEYWORD(OUTER);
        KEYWORD(INNER);
        KEYWORD(CROSS);
        KEYWORD(JOIN);
        KEYWORD(NOT);
        KEYWORD(INDEXED);
        KEYWORD(AS);
        KEYWORD(VALUES);
        KEYWORD(GROUP);
        KEYWORD(HAVING);
        KEYWORD(SELECT);
        KEYWORD(FROM);
        KEYWORD(WHERE);
        KEYWORD(EXPLAIN);
        KEYWORD(QUERY);
        KEYWORD(PLAN);
        KEYWORD(WITH);
        KEYWORD(ORDER);
        KEYWORD(BY);
        KEYWORD(LIMIT);
        KEYWORD(DISTINCT);
        KEYWORD(ALL);
        KEYWORD(OFFSET);
        KEYWORD(RECURSIVE);
        KEYWORD(INTERSECT);
        KEYWORD(EXCEPT);
        KEYWORD(UNION);
        KEYWORD(COLLATE);
        KEYWORD(ASC);
        KEYWORD(DESC);
        KEYWORD(ROLLBACK);
        KEYWORD(ABORT);
        KEYWORD(REPLACE);
        KEYWORD(FAIL);
        KEYWORD(IGNORE);
        KEYWORD(OR);
        KEYWORD(UPDATE);
        KEYWORD(SET);
        KEYWORD(CREATE);
        KEYWORD(TABLE);
        KEYWORD(TEMP);
        KEYWORD(TEMPORARY);
        KEYWORD(IF);
        KEYWORD(EXISTS);
        KEYWORD(WITHOUT);
        KEYWORD(ROWID);
        KEYWORD(CONSTRAINT);
        KEYWORD(PRIMARY);
        KEYWORD(KEY);
        KEYWORD(AUTOINCREMENT);
        KEYWORD(UNIQUE);
        KEYWORD(CHECK);
        KEYWORD(DEFAULT);
        KEYWORD(NULL);
        KEYWORD(CURRENT_TIME);
        KEYWORD(CURRENT_DATE);
        KEYWORD(CURRENT_TIMESTAMP);
        KEYWORD(CONFLICT);
        KEYWORD(CASCADE);
        KEYWORD(RESTRICT);
        KEYWORD(NO);
        KEYWORD(ACTION);
        KEYWORD(MATCH);
        KEYWORD(DEFERRABLE);
        KEYWORD(INITIALLY);
        KEYWORD(DEFERRED);
        KEYWORD(IMMEDIATE);
        KEYWORD(DELETE);
        KEYWORD(REFERENCES);
        KEYWORD(AND);
    }

    typedef qi::rule<Iterator, Skipper> rule;

    rule sql_stmt_list;
    rule sql_stmt ;
    rule explain_stmt ;
    rule stmt ;
    rule create_table_stmt ;
    rule column_def ;
    rule type_name ;
    rule column_constraint ;
    rule literal_value ;
    rule conflict_clause ;
    rule foreign_key_clause ;
    rule table_constraint ;
    rule indexed_column ;
    rule numeric_literal ;
    rule string_literal ;
    rule blob_literal ;
    rule select_stmt ;
    rule order_by_clause ;
    rule limit_clause ;
    rule select_phrase ;
    rule result_column ;
    rule values_clause ;
    rule table_or_subquery ;
    rule table_clause ;
    rule composite_table_name ;
    rule index_clause ;
    rule join_clause ;
    rule join_operator ;
    rule join_constraint ;
    rule with_clause ;
    rule common_table_expression ;
    rule compound_operator ;
    rule ordering_term ;
    rule update_stmt ;
    rule qualified_table_name ;
    rule update_limited_clause ;
    rule weasel_clause ;
    rule expr ;
    rule or_oper ;
    rule and_oper ;
    rule comparison_operator ;
    rule compare_oper ;
    rule ineq_operator ;
    rule ineq_oper ;
    rule bitwise_operator ;
    rule bitwise_oper ;
    rule term ;
    rule factor ;
    rule composite_column_name ;
    rule bind_parameter ;
    rule function_name ;
    rule foreign_table ;
    rule index_name ;
    rule table_name ;
    rule database_name ;
    rule collation_name ;
    rule column_name ;
    rule table_alias ;
    rule column_alias ;
    rule identifier ;
    rule tcl_identifier ;
    rule name ;


    DECL_KEYWORD(IS);
    DECL_KEYWORD(IN);
    DECL_KEYWORD(LIKE);
    DECL_KEYWORD(GLOB);
    DECL_KEYWORD(REGEXP);
    DECL_KEYWORD(FOREIGN);
    DECL_KEYWORD(ON);
    DECL_KEYWORD(USING);
    DECL_KEYWORD(NATURAL);
    DECL_KEYWORD(LEFT);
    DECL_KEYWORD(OUTER);
    DECL_KEYWORD(INNER);
    DECL_KEYWORD(CROSS);
    DECL_KEYWORD(JOIN);
    DECL_KEYWORD(NOT);
    DECL_KEYWORD(INDEXED);
    DECL_KEYWORD(AS);
    DECL_KEYWORD(VALUES);
    DECL_KEYWORD(GROUP);
    DECL_KEYWORD(HAVING);
    DECL_KEYWORD(SELECT);
    DECL_KEYWORD(FROM);
    DECL_KEYWORD(WHERE);
    DECL_KEYWORD(EXPLAIN);
    DECL_KEYWORD(QUERY);
    DECL_KEYWORD(PLAN);
    DECL_KEYWORD(WITH);
    DECL_KEYWORD(ORDER);
    DECL_KEYWORD(BY);
    DECL_KEYWORD(LIMIT);
    DECL_KEYWORD(DISTINCT);
    DECL_KEYWORD(ALL);
    DECL_KEYWORD(OFFSET);
    DECL_KEYWORD(RECURSIVE);
    DECL_KEYWORD(INTERSECT);
    DECL_KEYWORD(EXCEPT);
    DECL_KEYWORD(UNION);
    DECL_KEYWORD(COLLATE);
    DECL_KEYWORD(ASC);
    DECL_KEYWORD(DESC);
    DECL_KEYWORD(ROLLBACK);
    DECL_KEYWORD(ABORT);
    DECL_KEYWORD(REPLACE);
    DECL_KEYWORD(FAIL);
    DECL_KEYWORD(IGNORE);
    DECL_KEYWORD(OR);
    DECL_KEYWORD(UPDATE);
    DECL_KEYWORD(SET);
    DECL_KEYWORD(CREATE);
    DECL_KEYWORD(TABLE);
    DECL_KEYWORD(TEMP);
    DECL_KEYWORD(TEMPORARY);
    DECL_KEYWORD(IF);
    DECL_KEYWORD(EXISTS);
    DECL_KEYWORD(WITHOUT);
    DECL_KEYWORD(ROWID);
    DECL_KEYWORD(CONSTRAINT);
    DECL_KEYWORD(PRIMARY);
    DECL_KEYWORD(KEY);
    DECL_KEYWORD(AUTOINCREMENT);
    DECL_KEYWORD(UNIQUE);
    DECL_KEYWORD(CHECK);
    DECL_KEYWORD(DEFAULT);
    DECL_KEYWORD(NULL);
    DECL_KEYWORD(CURRENT_TIME);
    DECL_KEYWORD(CURRENT_DATE);
    DECL_KEYWORD(CURRENT_TIMESTAMP);
    DECL_KEYWORD(CONFLICT);
    DECL_KEYWORD(CASCADE);
    DECL_KEYWORD(RESTRICT);
    DECL_KEYWORD(NO);
    DECL_KEYWORD(ACTION);
    DECL_KEYWORD(MATCH);
    DECL_KEYWORD(DEFERRABLE);
    DECL_KEYWORD(INITIALLY);
    DECL_KEYWORD(DEFERRED);
    DECL_KEYWORD(IMMEDIATE);
    DECL_KEYWORD(DELETE);
    DECL_KEYWORD(REFERENCES);
    DECL_KEYWORD(AND);
};

template< typename Iterator, typename Skipper>
bool phrase_parse( Iterator first, Iterator last, const Skipper &skipper)
{
    SqlGrammar<Iterator, Skipper> g;
    return qi::phrase_parse( first, last, g, skipper);
}

void test( const std::string &filename)
{
    std::string buffer;
    std::ifstream inputfile( filename);

    size_t count = 0;
    while (getline( inputfile, buffer))
    {
        try
        {
            if (!phrase_parse( buffer.cbegin(), buffer.cend(), space | "/*" >> *(char_ - "*/") >> "*/"))
            {
                std::cout << buffer << '\n';
            }
            else
            {
                ++count;
            }
        }
        catch (std::exception &e)
        {
            std::cerr << "****\n";
            std::cerr << "Parsing failure: " << e.what() << '\n';
            std::cout << buffer << '\n';
        }
    }

    std::cout << count << " queries succeeded\n";
}

/*
 *
 */
int main(int argc, char** argv)
{

    if (argc > 1)
    {
        test( argv[1]);
    }

    return 0;
}

