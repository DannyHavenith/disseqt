/*
 * File:   disseqt.cpp
 * Author: danny
 *
 * Created on April 4, 2015, 11:31 PM
 */
#include <boost/spirit/include/qi.hpp>

using namespace boost::spirit;
using namespace boost::spirit::ascii;

#undef NULL
#define KEYWORD( kw_) rule kw_ = no_case[lit(#kw_)]

template< typename Iterator, typename Skipper>
struct SqlGrammar : qi::grammar<Iterator, Skipper>
{
    SqlGrammar()
    :SqlGrammar::base_type( sql_stmt)
    {
    }

    typedef qi::rule<Iterator, Skipper> rule;


    rule sql_stmt_list
        =   sql_stmt % ';'
        ;


    rule sql_stmt =
            explain_stmt
        |   stmt
        ;

    rule explain_stmt =
            EXPLAIN > -( QUERY >> PLAN) >> stmt
        ;

    rule stmt = /* TODO: finish */
            update_stmt
        |   select_stmt
        |   create_table_stmt
        ;

    rule create_table_stmt =
            CREATE >> -(TEMP|TEMPORARY) >> TABLE
        >   -(IF >> NOT >> EXISTS)
        >>  -(database_name >> '.') >> table_name
        >>  (
                (AS > select_stmt)
            |   ('(' >> column_def%',' >> -(table_constraint%',') > ')' >> -( WITHOUT >> ROWID))
            )
        ;

    rule column_def =
            column_name >> -type_name >> *column_constraint
        ;

    rule type_name =
            *identifier >> -( '(' > qi::int_ >> -(',' >> int_ ) > ')'   )
        ;

    rule column_constraint =
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

    rule literal_value =
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
    rule conflict_clause =
            ON > CONFLICT > (ROLLBACK|ABORT|FAIL|IGNORE|REPLACE)
        ;

    rule foreign_key_clause =
            REFERENCES >> foreign_table >> -( '(' > column_name%',' > ')')
        >>  *(
                (ON > (DELETE|UPDATE) > (SET >> NULL | SET >> DEFAULT | CASCADE | RESTRICT | NO >> ACTION))
            |   (MATCH >> name)
            )
        >>  -(-NOT >> DEFERRABLE >> -(INITIALLY >> (DEFERRED|IMMEDIATE)))
        ;

    rule table_constraint =
            -(CONSTRAINT > name)
        >>  (
                ((PRIMARY >> KEY|UNIQUE) > '(' >> indexed_column%',' >> ')' >> conflict_clause)
            |   (CHECK > '(' > expr > ')')
            |   (FOREIGN > KEY > '(' > column_name%',' > ')' > foreign_key_clause)
            )
        ;

    rule indexed_column =
            column_name >> -(COLLATE > collation_name) >> -(ASC|DESC)
        ;

    rule numeric_literal =
            (+digit || '.' >> *digit) >> -( 'E' > -(char_('+')|'-') > +digit)
        |   "0x" >> +xdigit;
        ;

    rule string_literal =
            '\'' >> lexeme[char_ - '\''] >> '\''
        ;

    rule blob_literal =
            no_case['x'] >> string_literal
        ;

    rule select_stmt =
            -with_clause
        >>  ( select_phrase | values_clause) % compound_operator
        >> -order_by_clause
        >> -limit_clause
       ;

    rule order_by_clause =
            ORDER >> BY >> ordering_term % ','
        ;

    rule limit_clause =
            LIMIT >> expr >> -((OFFSET|',') >> expr)
        ;


    rule select_phrase =
            SELECT > -(DISTINCT|ALL) > result_column%','
        >>  -(FROM > (join_clause| table_or_subquery%','))
        >>  -(WHERE > expr)
        >>  -(GROUP > BY > expr % ',' > -(HAVING > expr))
        ;

    rule result_column =
            '*'
        |   table_name >> '.' >> '*'
        |   expr >> -( -AS >> column_alias)
        ;

    rule values_clause =
            VALUES > ('(' >> expr % ',' >> ')') % ','
        ;

    rule table_or_subquery =
            table_clause
        |   '(' >> (table_or_subquery%',' | join_clause) >> ')'
        |   '(' >> select_stmt >> ')' >> -( -AS >> table_alias)
        ;

    rule table_clause =
            -(database_name >> '.') >> table_name
        >>  -( -AS >> table_alias)
        >>  -index_clause
        ;

    rule index_clause =
            NOT >> INDEXED | INDEXED >> BY >> index_name
        ;

    rule join_clause =
            table_or_subquery >> *(join_operator >> table_or_subquery >> join_constraint)
        ;

    rule join_operator =
            ','
        |   -NATURAL >> -( LEFT >> -OUTER | INNER | CROSS) >> JOIN
        ;

    rule join_constraint =
            ON >> expr
        |   USING >> '(' >> column_name%',' >> ')'
        |   qi::eps
        ;

    rule with_clause =
            WITH > -RECURSIVE >> common_table_expression %','
        ;

    rule common_table_expression =
            table_name >> -( '(' >> column_name%',' >> ')')
        >>  AS >> '(' >> select_stmt >> ')';
        ;

    rule compound_operator =
            UNION >> -ALL
        |   INTERSECT
        |   EXCEPT
        ;

    rule ordering_term =
            expr >> -(COLLATE > collation_name) >> -(ASC | DESC)
        ;

    rule update_stmt =
            -with_clause >> UPDATE >> -weasel_clause >> qualified_table_name
        >>  SET >> (column_name >> '=' > expr)%',' >> (WHERE > expr)
        >>  -update_limited_clause
        ;

    rule qualified_table_name =
            -(database_name >> '.') >> table_name >> -index_clause
        ;

    rule update_limited_clause =
            -order_by_clause >> limit_clause
        ;

    rule weasel_clause =
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
    rule expr =
            or_oper >> *( OR > or_oper)
        ;

    rule or_oper =
            and_oper >> *( AND > and_oper)
        ;

    rule and_oper =
            compare_oper >> *(comparison_operator >> compare_oper)
        ;

    rule comparison_operator =
            lit("==") | "=" | "!=" |  "<>" | IS >> NOT | IS | IN | LIKE | GLOB | MATCH | REGEXP
        ;

    rule compare_oper =
            ineq_oper >> *( ineq_operator >> ineq_oper)
        ;

    rule ineq_operator =
            lit("<=") | '<' | ">=" | '>'
        ;

    rule ineq_oper =
            bitwise_oper >> *(bitwise_operator >> bitwise_oper)
        ;

    rule bitwise_operator =
            lit("<<") | ">>" | '&' | '|'
        ;

    rule bitwise_oper =
            term >>  *('+' >> term |'-' >> term)
        ;

    rule term =
           factor >> *( '*' >> factor | '/' >> factor)
        ;

    rule factor =
            literal_value
        |   bind_parameter
        |   (function_name >> '(' > -( '*'| -DISTINCT >> expr%','))
        |   database_name >> '.' >> table_name >> '.' >> column_name // todo: make more efficient
        |   table_name >> '.' >> column_name
        |   column_name
        |   '(' >> expr >> ')'
        |   ('-' >> factor)
        |   ('+' >> factor)
        ;

    rule bind_parameter =
            lexeme[ char_('?') >> *digit]
        |   lexeme[ char_(':') >> (alpha|char_('_')) >> *(alnum|char_('_'))]
        |   lexeme[ char_('$') >> ((alpha|char_('_')) >> *(alnum|char_('_')))%"::" >> -( '(' >> *(char_ - ')') >> ')')]
        ;

    rule function_name = identifier
        ;
    rule foreign_table = name
        ;
    rule index_name = name
        ;
    rule table_name = name
        ;
    rule database_name = name
        ;
    rule collation_name = name
        ;
    rule column_name = name
        ;
    rule table_alias = name
        ;
    rule column_alias = name
        ;

    rule identifier =
            (alpha|char_('_')) >> *(alnum|char_('_'))
        ;

    rule tcl_identifier =
            (alpha|char_('_')) >> *(alnum|char_('_'))%"::" >> -( '(' >> *(char_ - ')') >> ')')
        ;

    // names can be bare identifiers, but also be quoted.
    rule name =
            identifier
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
};

template< typename Iterator, typename Skipper>
bool phrase_parse( Iterator first, Iterator last, const Skipper &skipper)
{
    SqlGrammar<Iterator, Skipper> g;
    return qi::phrase_parse( first, last, g, skipper);
}

/*
 *
 */
int main(int argc, char** argv)
{


    std::string shouldWork( "SELECT 1,2,    3,4 FROM WHERE");
    bool result = phrase_parse( shouldWork.cbegin(), shouldWork.cend(), space);
    if (result)
    {
        std::cout << "success\n";
    }
    else
    {
        std::cout << "failure\n";
    }

    return 0;
}

