/*
 * File:   disseqt.cpp
 * Author: danny
 *
 * Created on April 4, 2015, 11:31 PM
 */

//#if !defined(BOOST_SPIRIT_DEBUG_OUT)
//#define BOOST_SPIRIT_DEBUG_OUT std::cerr
//#endif
//
////  number of tokens to print while debugging
//#if !defined(BOOST_SPIRIT_DEBUG_PRINT_SOME)
//#define BOOST_SPIRIT_DEBUG_PRINT_SOME 20
//#endif
//
////  number of spaces to indent
//#if !defined(BOOST_SPIRIT_DEBUG_INDENT)
//#define BOOST_SPIRIT_DEBUG_INDENT 2
//#endif

#include <string>
#include <iostream>
#include <fstream>
#include <boost/spirit/include/qi.hpp>
//#include <boost/phoenix.hpp>

#include "lexer.h"

using namespace boost::spirit;
using namespace boost::spirit::ascii;

#define DISSEQT_DEBUG

#undef NULL
#if defined( DISSEQT_DEBUG)
#    define DISSEQT_DEBUG_NODE( node_) node_.name( #node_); qi::debug( node_)
#else
#    define DISSEQT_DEBUG_NODE( node_) node_.name( #node_)
#endif

#define KEYWORD( kw_) kw_ = lexeme[no_case[lit(#kw_)]]; DISSEQT_DEBUG_NODE( kw_)
#define DECL_KEYWORD( kw_ ) rule kw_


template< typename Iterator, typename Skipper>
struct SqlGrammar : qi::grammar<Iterator, Skipper>
{
    SqlGrammar()
    :SqlGrammar::base_type( sql_stmt)
    {
        namespace ph=boost::phoenix;

        sql_stmt_list
            =   sql_stmt % ';'
            ;

        sql_stmt =
                explain_stmt
            |   stmt
            ;
        DISSEQT_DEBUG_NODE( sql_stmt);

        explain_stmt =
                EXPLAIN > -( QUERY >> PLAN) >> stmt
            ;
        DISSEQT_DEBUG_NODE( explain_stmt);

        stmt =  /* TODO: finish */
                update_stmt
            |   select_stmt
            |   create_table_stmt
            |   insert_stmt
            ;
        DISSEQT_DEBUG_NODE( stmt);

        insert_stmt =
                -with_clause
            >>  (REPLACE | INSERT >> -weasel_clause) >   INTO
            >   composite_table_name >> -column_list
            >>  (
                    values_clause
                |   select_stmt
                |   (DEFAULT > VALUES)
                )
            ;
        DISSEQT_DEBUG_NODE( insert_stmt);

        create_table_stmt =
                CREATE >> -(TEMP|TEMPORARY) >> TABLE
            >   -(IF > NOT > EXISTS)
            >>  composite_table_name
            >>  (
                    (AS > select_stmt)
                |   ('(' >> column_def%',' >> -(table_constraint%',') > ')' >> -( WITHOUT > ROWID))
                )
            ;
        DISSEQT_DEBUG_NODE( create_table_stmt);

        column_def =
                column_name >> -type_name >> *column_constraint
            ;
        DISSEQT_DEBUG_NODE( column_def);

        type_name =
                *identifier >> -( '(' > qi::int_ >> -(',' >> int_ ) > ')'   )
            ;
        DISSEQT_DEBUG_NODE( type_name);

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
        DISSEQT_DEBUG_NODE( column_constraint);

        literal_value =
                numeric_literal
            |   string_literal
            |   blob_literal
            |   NULL
            |   CURRENT_TIME
            |   CURRENT_DATE
            |   CURRENT_TIMESTAMP
            ;
        DISSEQT_DEBUG_NODE( literal_value);

        // in the syntax spec for SQLite, this one has an empty alternative, this has been
        // removed and the conflict clause itself has become optional in all rules that use it.

        conflict_clause =
                ON > CONFLICT > (ROLLBACK|ABORT|FAIL|IGNORE|REPLACE)
            ;
        DISSEQT_DEBUG_NODE( conflict_clause);

        foreign_key_clause =
                REFERENCES >> foreign_table >> -column_list
            >>  *(
                    (ON > (DELETE|UPDATE) > (SET >> NULL | SET >> DEFAULT | CASCADE | RESTRICT | NO >> ACTION))
                |   (MATCH >> name)
                )
            >>  -(-NOT >> DEFERRABLE >> -(INITIALLY > (DEFERRED|IMMEDIATE)))
            ;
        DISSEQT_DEBUG_NODE( foreign_key_clause);

        table_constraint =
                -(CONSTRAINT > name)
            >>  (
                    ((PRIMARY >> KEY|UNIQUE) > '(' >> indexed_column%',' >> ')' >> -conflict_clause)
                |   (CHECK > '(' > expr > ')')
                |   (FOREIGN > KEY > column_list > foreign_key_clause)
                )
            ;
        DISSEQT_DEBUG_NODE( table_constraint);

        indexed_column =
                column_name >> -(COLLATE > collation_name) >> -(ASC|DESC)
            ;
        DISSEQT_DEBUG_NODE( indexed_column);

        numeric_literal =
                lexeme[ (+digit || '.' >> *digit) >> -( 'E' >> -(char_('+')|'-') >> +digit)]
            |   lexeme[ "0x" >> +xdigit]
            ;
        DISSEQT_DEBUG_NODE( numeric_literal);
            ;

        string_literal =
                '\'' >> lexeme[*(char_ - '\'')] >> '\''
            ;
        DISSEQT_DEBUG_NODE( string_literal);

        blob_literal =
                no_case['x'] >> string_literal
            ;
        DISSEQT_DEBUG_NODE( blob_literal);

        select_stmt =
                -with_clause
            >>  ( select_phrase | values_clause) % compound_operator
            >> -order_by_clause
            >> -limit_clause
           ;
        DISSEQT_DEBUG_NODE( select_stmt);

        order_by_clause =
                ORDER >> BY >> ordering_term % ','
            ;
        DISSEQT_DEBUG_NODE( order_by_clause);

        limit_clause =
                LIMIT >> expr >> -((OFFSET|',') >> expr)
            ;
        DISSEQT_DEBUG_NODE( limit_clause);

        select_phrase =
                SELECT > -(DISTINCT|ALL) > result_column%','
            >>  -(FROM > (join_clause| table_or_subquery%','))
            >>  -(WHERE > expr)
            >>  -(GROUP > BY > expr % ',' > -(HAVING > expr))
            ;

        DISSEQT_DEBUG_NODE( select_phrase);

        result_column =
                '*'
            |   expr >> -( -AS >> column_alias)
            |   table_name >> '.' >> '*'
            ;
        DISSEQT_DEBUG_NODE( result_column);

        values_clause =
                VALUES > ('(' >> expr % ',' >> ')') % ','
            ;
        DISSEQT_DEBUG_NODE( values_clause);

        table_or_subquery =
                table_clause
            |   '(' >> (table_or_subquery%',' | join_clause) >> ')'
            |   '(' >> select_stmt >> ')' >> -( -AS >> table_alias)
            ;
        DISSEQT_DEBUG_NODE( table_or_subquery);

        table_clause =
                composite_table_name
            >>  -( -AS >> table_alias)
            >>  -index_clause
            ;
        DISSEQT_DEBUG_NODE( table_clause);

        composite_table_name =
                -(database_name >> '.') >> table_name
            ;

        DISSEQT_DEBUG_NODE( composite_table_name);

        index_clause =
                NOT >> INDEXED | INDEXED >> BY >> index_name
            ;
        DISSEQT_DEBUG_NODE( index_clause);

        join_clause =
                table_or_subquery >> *(join_operator >> table_or_subquery >> -join_constraint)
            ;
        DISSEQT_DEBUG_NODE( join_clause);

        join_operator =
                ','
            |   -NATURAL >> -( LEFT >> -OUTER | INNER | CROSS) >> JOIN
            ;
        DISSEQT_DEBUG_NODE( join_operator);

        // officially contains an empty (epsilon) alternative, but instead we're making this
        // rule optional wherever it is used.

        join_constraint =
                (ON > expr)
            |   (USING > column_list)
            ;        DISSEQT_DEBUG_NODE( join_constraint);

        column_list =
                '(' > column_name%',' > ')'
            ;
        DISSEQT_DEBUG_NODE( column_list);

        with_clause =
                WITH > -RECURSIVE >> common_table_expression %','
            ;

        DISSEQT_DEBUG_NODE( with_clause);

        common_table_expression =
                table_name >> -column_list
            >>  AS >> '(' >> select_stmt >> ')';
        DISSEQT_DEBUG_NODE( common_table_expression);
            ;

        compound_operator =
                UNION >> -ALL
            |   INTERSECT
            |   EXCEPT
            ;
        DISSEQT_DEBUG_NODE( compound_operator);

        ordering_term =
                expr >> -(COLLATE > collation_name) >> -(ASC | DESC)
            ;
        DISSEQT_DEBUG_NODE( ordering_term);

        update_stmt =
                -with_clause >> UPDATE > -weasel_clause >> qualified_table_name
            >   SET >> (column_name >> '=' > expr)%',' >> -(WHERE > expr)
            >>  -update_limited_clause
            ;
        DISSEQT_DEBUG_NODE( update_stmt);

        qualified_table_name =
                composite_table_name >> -index_clause
            ;
        DISSEQT_DEBUG_NODE( qualified_table_name);

        update_limited_clause =
                -order_by_clause >> limit_clause
            ;
        DISSEQT_DEBUG_NODE( update_limited_clause);

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
        DISSEQT_DEBUG_NODE( weasel_clause);

        expr =
                or_operand >> *( (OR-ORDER) >> or_operand) // really need tokenizer here.
            ;
        DISSEQT_DEBUG_NODE( expr);

        or_operand =
                and_operand >> *( AND >> and_operand)
            ;
        DISSEQT_DEBUG_NODE( or_operand);

        and_operand =
                compare_operand >> *comparison_rhs
            ;
        DISSEQT_DEBUG_NODE( and_operand);

        comparison_rhs =
                comparison_operator >> compare_operand
            |   -NOT >> BETWEEN >> compare_operand >> AND >> compare_operand
            |   ISNULL
            |   NOTNULL
            |   NOT >> NULL
            |   -NOT >> IN >>   (
                                    composite_table_name
                                |   ('(' > -(select_stmt | expr%',') > ')')
                                )
            ;
        DISSEQT_DEBUG_NODE( comparison_rhs);

        comparison_operator =
                lit("==") | "=" | "!=" |  "<>" | IS >> NOT | IS | LIKE | GLOB | MATCH | REGEXP
            ;

        DISSEQT_DEBUG_NODE( comparison_operator);

        compare_operand =
                ineq_operand >> *( ineq_operator >> ineq_operand)
            ;
        DISSEQT_DEBUG_NODE( compare_operand);

        ineq_operator =
                lit("<=") | '<' | ">=" | '>'
            ;
        DISSEQT_DEBUG_NODE( ineq_operator);

        ineq_operand =
                bitwise_operand >> *(bitwise_operator >> bitwise_operand)
            ;
        DISSEQT_DEBUG_NODE( ineq_operand);

        bitwise_operator =
                lit("<<") | ">>" | '&' | '|'
            ;
        DISSEQT_DEBUG_NODE( bitwise_operator);

        bitwise_operand =
                term >>  *('+' >> term |'-' >> term)
            ;
        DISSEQT_DEBUG_NODE( bitwise_operand);

        term =
               factor >> *( '*' >> factor | '/' >> factor | '%' >> factor)
            ;
        DISSEQT_DEBUG_NODE( term);

        factor =
                singular >> *( "||" >> singular)
            ;
        DISSEQT_DEBUG_NODE( factor);

        singular =
                (-NOT >> EXISTS > '(' >> select_stmt >> ')')
            |   (CASE > -expr >> +(WHEN > expr >> THEN > expr) >> -(ELSE > expr) >> END)
            |   (CAST > '(' > expr > AS > type_name > ')')
            |   literal_value
            |   bind_parameter
            |   (function_name >> '(' > -( '*'| -DISTINCT >> expr%',') >> ')')
            |   composite_column_name
            |   '(' >> select_stmt >> ')'   // not in the syntax diagrams, but described in "Table Column Names".
            |   '(' >> expr >> ')'
            |   ('-' >> singular)
            |   ('+' >> singular)
            |   ('~' >> singular)
            ;
        DISSEQT_DEBUG_NODE( singular);

        // todo: make more efficient

        composite_column_name =
                database_name >> '.' >> table_name >> '.' >> column_name
            |   table_name >> '.' >> column_name
            |   column_name
            ;
        DISSEQT_DEBUG_NODE( composite_column_name);

        bind_parameter =
                lexeme[ char_('?') >> *digit]
            |   lexeme[ char_(':') >> (alpha|char_('_')) >> *(alnum|char_('_'))]
            |   lexeme[ char_('$') >> ((alpha|char_('_')) >> *(alnum|char_('_')))%"::" >> -( '(' >> *(char_ - ')') >> ')')]
            ;
        DISSEQT_DEBUG_NODE( bind_parameter);

        function_name =  name.alias()
            ;
        DISSEQT_DEBUG_NODE( function_name);

        foreign_table =  name.alias()
            ;
        DISSEQT_DEBUG_NODE( foreign_table);

        index_name =  name.alias()
            ;
        DISSEQT_DEBUG_NODE( index_name);

        table_name =  name.alias()
            ;
        DISSEQT_DEBUG_NODE( table_name);

        database_name =  name.alias()
            ;
        DISSEQT_DEBUG_NODE( database_name);

        collation_name =  name.alias()
            ;
        DISSEQT_DEBUG_NODE( collation_name);

        column_name =  name.alias()
            ;
        DISSEQT_DEBUG_NODE( column_name);

        table_alias =  name.alias()
            ;
        DISSEQT_DEBUG_NODE( table_alias);

        column_alias =  name.alias()
            ;
        DISSEQT_DEBUG_NODE( column_alias);

        identifier =
                lexeme[(alpha|char_('_')) >> *(alnum|char_('_'))]
            ;
        DISSEQT_DEBUG_NODE( identifier);

        tcl_identifier =
                lexeme[(alpha|char_('_')) >> *(alnum|char_('_'))%"::" >> -( '(' >> *(char_ - ')') >> ')')]
            ;
        DISSEQT_DEBUG_NODE( tcl_identifier);

        // names can be bare identifiers, but also be quoted.
        // this would be a lot simpler (and faster) if we'd use a tokenizer.
        name =
                identifier.alias() - (FROM|WHERE|GROUP|JOIN|WHEN|THEN|ELSE|END|LEFT|LIMIT|ON|UNION|ORDER|LIMIT|WHEN|CAST|ORDER|BY|LIMIT|DISTINCT|CASE)
            ;
        DISSEQT_DEBUG_NODE( name);

//        qi::on_error<qi::fail>
//                    (
//                        sql_stmt
//                      , ph::ref( std::cerr)
//                            << "Error! Expecting "
//                            << _4                               // what failed?
//                            << " here: \""
//                            << ph::construct<std::string>(_3, _2)   // iterators to error-pos, end
//                            << "\""
//                            << std::endl
//                    );

        KEYWORD(CAST);
        KEYWORD(ISNULL);
        KEYWORD(NOTNULL);
        KEYWORD(CASE);
        KEYWORD(WHEN);
        KEYWORD(THEN);
        KEYWORD(ELSE);
        KEYWORD(END);
        KEYWORD(INSERT);
        KEYWORD(INTO);
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
        KEYWORD(BETWEEN);
    }

    typedef qi::rule<Iterator, Skipper> rule;

    rule comparison_rhs;
    rule column_list;
    rule insert_stmt;
    rule sql_stmt_list;
    rule sql_stmt;
    rule explain_stmt;
    rule stmt;
    rule create_table_stmt;
    rule column_def;
    rule type_name;
    rule column_constraint;
    rule literal_value;
    rule conflict_clause;
    rule foreign_key_clause;
    rule table_constraint;
    rule indexed_column;
    rule numeric_literal;
    rule string_literal;
    rule blob_literal;
    rule select_stmt;
    rule order_by_clause;
    rule limit_clause;
    rule select_phrase;
    rule result_column;
    rule values_clause;
    rule table_or_subquery;
    rule table_clause;
    rule composite_table_name;
    rule index_clause;
    rule join_clause;
    rule join_operator;
    rule join_constraint;
    rule with_clause;
    rule common_table_expression;
    rule compound_operator;
    rule ordering_term;
    rule update_stmt;
    rule qualified_table_name;
    rule update_limited_clause;
    rule weasel_clause;
    rule expr;
    rule or_operand;
    rule and_operand;
    rule comparison_operator;
    rule compare_operand;
    rule ineq_operator;
    rule ineq_operand;
    rule bitwise_operator;
    rule bitwise_operand;
    rule term;
    rule singular;
    rule factor;
    rule composite_column_name;
    rule bind_parameter;
    rule function_name;
    rule foreign_table;
    rule index_name;
    rule table_name;
    rule database_name;
    rule collation_name;
    rule column_name;
    rule table_alias;
    rule column_alias;
    rule identifier;
    rule tcl_identifier;
    rule name;

    DECL_KEYWORD(CAST);
    DECL_KEYWORD(ISNULL);
    DECL_KEYWORD(NOTNULL);
    DECL_KEYWORD(CASE);
    DECL_KEYWORD(WHEN);
    DECL_KEYWORD(THEN);
    DECL_KEYWORD(ELSE);
    DECL_KEYWORD(END);
    DECL_KEYWORD(INSERT);
    DECL_KEYWORD(INTO);
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
    DECL_KEYWORD(BETWEEN);
};

template< typename Iterator, typename Skipper>
bool phrase_parse( Iterator first, Iterator last, const Skipper &skipper)
{
    SqlGrammar<Iterator, Skipper> g;
    return qi::phrase_parse( first, last, g, skipper);
}

struct printer
{
    typedef boost::spirit::utf8_string string;

    void element(string const& tag, string const& value, int depth) const
    {
        for (int i = 0; i < (depth*4); ++i) // indent to depth
            std::cout << ' ';

        std::cout << "tag: " << tag;
        if (value != "")
            std::cout << ", value: " << value;
        std::cout << std::endl;
    }
};

void print_info(boost::spirit::info const& what)
{
    using boost::spirit::basic_info_walker;

    printer pr;
    basic_info_walker<printer> walker(pr, what.tag, 0);
    boost::apply_visitor(walker, what.value);
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
                std::cout << "**********\n";
                std::cout << buffer << '\n';
                std::cout << "**********\n";
            }
            else
            {
                ++count;
            }
        }
        catch ( qi::expectation_failure<std::string::const_iterator> &e)
        {
            std::cerr << "***\n";
            std::cerr << "expectation failure:\n";
            std::cerr << std::string( buffer.cbegin(), e.first) << '\n';
            std::cerr << "<HERE>\n";
            std::cerr << std::string( e.first, e.last) << '\n';
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
//int main(int argc, char** argv)
//{
//
//    if (argc > 1)
//    {
//        test( argv[1]);
//    }
//
//    return 0;
//}

