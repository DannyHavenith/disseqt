/*
 * File:   disseqt.cpp
 * Author: danny
 *
 * Created on April 4, 2015, 11:31 PM
 */
#include <boost/spirit/include/qi.hpp>

using namespace boost::spirit;
using namespace boost::spirit::ascii;

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
            alter_table_stmt
        |   select_stmt
        ;

    rule select_stmt =
            -with_clause
        >>  ( select_phrase | values_clause) % compound_operator
        >> -( ORDER >> BY >> ordering_term % ',')
        >> -( LIMIT >> expr >> -((OFFSET|',') >> expr))
       ;

    rule select_phrase =
            SELECT > -(DISTINCT|ALL) > result_column % ','
        >>  -(FROM > (join_clause| table_or_subquery %','))
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
        >>  -(NOT >> INDEXED | INDEXED >> BY >> index_name)
        ;


    rule join_clause;
    rule with_clause;
    rule compound_operator;
    rule ordering_term;
    rule expr;
    rule alter_table_stmt;



    rule index_name = identifier
        ;
    rule table_name = identifier
        ;
    rule database_name = identifier
        ;
    rule table_alias = identifier
        ;
    rule column_alias = identifier
        ;
    rule identifier =
            (alpha|char_('_')) >> *(alnum|char_('_'))
        ;

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

