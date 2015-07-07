/*
 * File:   disseqt.cpp
 * Author: danny
 *
 * Created on April 4, 2015, 11:31 PM
 */
#include "lexer.h"
#include "disseqt_grammar.h"

#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_matches.hpp>
#include <string>
#include <iostream>
#include <iomanip>

using namespace boost::spirit;
using namespace boost::spirit::ascii;

//#define DISSEQT_DEBUG

#if defined( DISSEQT_DEBUG)
#    define DISSEQT_DEBUG_NODE( node_) node_.name( #node_); qi::debug( node_)
#else
#    define DISSEQT_DEBUG_NODE( node_) node_.name( #node_)
#endif
#define DISSEQT_PARSER_KEYWORD_ALTERNATIVE( r, type, keyword) |  t.keyword

// some printf-style debug functions.
namespace {
    namespace spirit = boost::spirit;
    template <typename Expr, typename Iterator = spirit::unused_type>
       struct attribute_of_parser
       {
           typedef typename spirit::result_of::compile<
               spirit::qi::domain, Expr
           >::type parser_expression_type;

           typedef typename spirit::traits::attribute_of<
               parser_expression_type, spirit::unused_type, Iterator
           >::type type;
       };

    template <typename T>
    void display_attribute_of_parser(T const&)
    {
        typedef std::string::const_iterator Iterator;
        typedef
                typename attribute_of_parser<
                    T,
                    typename disseqt::LexerTypes<Iterator>::iterator_type
                    >::type attribute_type;
        std::cout << typeid(attribute_type).name() << std::endl;
    }

    struct debug_print
    {
        debug_print( const std::string &message)
        : message( message)
        {

        }

        template< typename A, typename B, typename C>
        void operator()( const A&, const B&, const C&) const
        {
            std::cout << "debug: " << message << std::endl; // make sure the string gets flushed...
        }

        std::string message;
    };
}

namespace disseqt
{
namespace {

    /// Utility function to create a ternary expression. Currently only used for the
    /// <e1> BETWEEN <e2> AND <e3>
    ast::expression ternaryexp(
            ast::operator_type op,
            const ast::expression &e1,
            const ast::expression &e2,
            const ast::expression &e3,
            bool negate = false
            )
    {
        if (negate)
        {
            return ast::unary_op{ ast::Not, ternaryexp( op, e1, e2, e3, false)};
        }
        else
        {
            return ast::ternary_op{ op, e1, e2, e3};
        }
    }

    /// construct a binary expression
    ast::expression binexp(
            ast::operator_type op,
            const ast::expression &left,
            const ast::expression &right)
    {
        return ast::binary_op{ op, left, right};
    }


    ast::expression unaryexp( ast::operator_type op, const ast::expression &e, bool negate = false)
    {
        if (negate)
        {
            return ast::unary_op{ ast::Not, unaryexp( op, e, false)};
        }
        else
        {
            return ast::unary_op{ op, e};
        }
    }

    // construct an IN expression, as in <e> IN <set>
    ast::expression inexp( const ast::expression &e, const ast::set_expression &set, bool negate = false)
    {
        if (negate)
        {
            return ast::unary_op{ ast::Not, inexp( e, set, false)};
        }
        else
        {
            return ast::in{ e, set};
        }
    }

    ast::expression collateexp( const ast::expression &e, const ast::collation_name &name)
    {
        return ast::collate{ e, name};
    }

}

template< typename Iterator, typename Skipper>
template< typename Tokens>
SqlGrammar<Iterator, Skipper>::SqlGrammar( const Tokens &t)
:SqlGrammar<Iterator, Skipper>::base_type( sql_stmt)
{
    namespace ph=boost::phoenix;
    using namespace ast;

    sql_stmt_list
    =   sql_stmt % ';'
    ;

    sql_stmt =
            explain_stmt
            |   stmt
            ;
    DISSEQT_DEBUG_NODE( sql_stmt);

    explain_stmt =
            t.EXPLAIN > -( t.QUERY >> t.PLAN) >> stmt
            ;
    DISSEQT_DEBUG_NODE( explain_stmt);

    stmt =  /* TODO: finish */
            update_stmt
        |   select_stmt
        |   create_table_stmt
        |   insert_stmt
        ;
    DISSEQT_DEBUG_NODE( stmt);

    display_attribute_of_parser(
            -with_clause
        >>  insert_type >> omit[t.INTO]
        >>  composite_table_name
        >>  -column_list
        >>  insert_values);

    insert_stmt =
            -with_clause
        >>  insert_type >> omit[t.INTO]
        >>  composite_table_name
        >>  -column_list
        >>  insert_values
        ;
    DISSEQT_DEBUG_NODE( insert_stmt);

    // insert_type is "REPLACE" or "INSERT" with an optional weasel clause ("OR ABORT|ROLLBACK, etc.").
    insert_type =
                omit[t.REPLACE] >> attr( Replace)
            |   t.INSERT >> weasel_clause
            ;
    DISSEQT_DEBUG_NODE( insert_type);

    insert_values =
            values_clause
        |   select_stmt
        |   (t.DEFAULT > t.VALUES > attr( default_values{}))
        ;
    DISSEQT_DEBUG_NODE( insert_values);

    create_table_stmt =
                t.CREATE >> matches[t.TEMP|t.TEMPORARY] >> t.TABLE
            >   -(t.IF > t.NOT > t.EXISTS)
            >>  composite_table_name
            >>  (
                    (t.AS > select_stmt)
                    |   ('(' >> column_def%',' >> -(',' >> table_constraint%',') > ')' >> -( t.WITHOUT > t.IDENTIFIER))
            )
            ;
    DISSEQT_DEBUG_NODE( create_table_stmt);

    column_def =
            column_name >> -type_name >> *column_constraint
            ;
    DISSEQT_DEBUG_NODE( column_def);

    type_name =
            *name >> -( '(' > signed_number >> -(',' >> signed_number ) > ')'   )
            ;
    DISSEQT_DEBUG_NODE( type_name);

    column_constraint =
                -(t.CONSTRAINT > name)
            >>  (
                    (t.PRIMARY > t.KEY > -(t.ASC|t.DESC) >> -conflict_clause >> -t.AUTOINCREMENT)
                    |   (t.NOT > t.NULL_T > -conflict_clause)
                    |   (t.UNIQUE > -conflict_clause)
                    |   (t.CHECK > '(' > expr > ')')
                    |   (t.DEFAULT > (signed_number|literal_value| ('(' > expr > ')')))
                    |   (t.COLLATE > collation_name)
                    |   foreign_key_clause
            )
            ;
    DISSEQT_DEBUG_NODE( column_constraint);

    literal_value =
                numeric_literal
            |   string_literal
            |   blob_literal
            |   t.NULL_T
            |   t.CURRENT_TIME
            |   t.CURRENT_DATE
            |   t.CURRENT_TIMESTAMP
            ;
    DISSEQT_DEBUG_NODE( literal_value);

    // in the syntax spec for SQLite, conflict_clause has an empty alternative, this has been
    // removed and the conflict clause itself has become optional in all rules that use it.
    conflict_clause =
            t.ON > t.CONFLICT > (t.ROLLBACK|t.ABORT|t.FAIL|t.IGNORE|t.REPLACE)
            ;
    DISSEQT_DEBUG_NODE( conflict_clause);

    foreign_key_clause =
            t.REFERENCES >> foreign_table >> -column_list
            >>  *(
                    (t.ON > (t.DELETE|t.UPDATE) > (t.SET >> t.NULL_T | t.SET >> t.DEFAULT | t.CASCADE | t.RESTRICT | t.NO >> t.ACTION))
                    |   (t.MATCH >> name)
            )
            >>  -(opt_not >> t.DEFERRABLE >> -(t.INITIALLY > (t.DEFERRED|t.IMMEDIATE)))
            ;
    DISSEQT_DEBUG_NODE( foreign_key_clause);

    table_constraint =
            -(t.CONSTRAINT > name)
            >>  (
                    ((t.PRIMARY >> t.KEY|t.UNIQUE) > '(' >> indexed_column%',' >> ')' >> -conflict_clause)
                    |   (t.CHECK > '(' > expr > ')')
                    |   (t.FOREIGN > t.KEY > column_list > foreign_key_clause)
            )
            ;
    DISSEQT_DEBUG_NODE( table_constraint);

    indexed_column =
            column_name >> -(t.COLLATE > collation_name) >> -(t.ASC|t.DESC)
            ;
    DISSEQT_DEBUG_NODE( indexed_column);

    numeric_literal =
            t.NUMERIC_LITERAL [ph::at_c<0>(_val) = _1]
            ;
    DISSEQT_DEBUG_NODE( numeric_literal);

    string_literal =
            t.STRING [ph::at_c<0>(_val) = _1]
            ;
    DISSEQT_DEBUG_NODE( string_literal);

    blob_literal =
            t.BLOB [ph::at_c<0>(_val) = _1]
            ;
    DISSEQT_DEBUG_NODE( blob_literal);

    select_stmt =
            -with_clause
            >>  compound_select
            >> -order_by_clause
            >> -limit_clause
            ;
    DISSEQT_DEBUG_NODE( select_stmt);

    compound_select =
                ( select_core | values_clause) % compound_operator
            ;
    DISSEQT_DEBUG_NODE( compound_select);

    order_by_clause =
            omit[t.ORDER >> t.BY] > ordering_term % ','
            ;
    DISSEQT_DEBUG_NODE( order_by_clause);

    limit_clause =
                t.LIMIT
            >   expr                 [ph::at_c<1>( _val) = _1]
            >>  (
                    t.OFFSET >> expr [ph::at_c<0>(_val) = _1]
                |   ','      >> expr [ph::at_c<0>(_val) = _1][ph::swap( ph::at_c<0>(_val), ph::at_c<1>(_val))]
                |   eps              [ph::at_c<0>(_val) = null{} ]
                )
            ;
    DISSEQT_DEBUG_NODE( limit_clause);

    // todo: implement distinct (as boolean?).
    select_core =
                t.SELECT > omit[-(t.DISTINCT|t.ALL)] > result_column%','
            >>  -(t.FROM > join_clause)
            >>  -(t.WHERE > expr)
            >>  -(omit[t.GROUP > t.BY] > expr % ',')
            >>  -(t.HAVING > expr)
            ;
    DISSEQT_DEBUG_NODE( select_core);

    result_column =
                '*' >> attr( star{})
            |   table_name >> '.' >> '*'
            |   expression_alias
            ;
    DISSEQT_DEBUG_NODE( result_column);

    expression_alias =
                expr >> -( -t.AS >> column_alias)
            ;
    DISSEQT_DEBUG_NODE( expression_alias);

    values_clause =
                t.VALUES > ('(' >> expr % ',' >> ')') % ','
            ;
    DISSEQT_DEBUG_NODE( values_clause);

    // this is a simplification: index clause can only appear after a
    // composite table name, not after a select
    table_or_subquery =
                 (composite_table_name |   '(' >> select_stmt >> ')')
             >>  -( -t.AS >> table_alias)
             >>  -index_clause
            ;
    DISSEQT_DEBUG_NODE( table_or_subquery);


    composite_table_name =
                -(database_name >> '.') >> table_name
            ;
    DISSEQT_DEBUG_NODE( composite_table_name);

    index_clause =
                omit[t.NOT >> t.INDEXED]
            |   omit[t.INDEXED >> t.BY] >> index_name
            ;
    DISSEQT_DEBUG_NODE( index_clause);

    join_clause =
                table_or_subquery
            >>  *(join_expression)
            ;
    DISSEQT_DEBUG_NODE( join_clause);

    join_expression =
                join_operator >> table_or_subquery >> -join_constraint
            ;
    DISSEQT_DEBUG_NODE( join_expression);

    join_operator =
                char_(',') [_val = ast::join_operator{false, Cross}]
            |       matches[t.NATURAL]
                >>  (
                        t.LEFT >> -t.OUTER >> attr( Left)
                    |   t.INNER            >> attr( Inner)
                    |   t.CROSS            >> attr( Cross)
                    |   eps                >> attr( Inner)
                    )
                >>  t.JOIN
            ;
    DISSEQT_DEBUG_NODE( join_operator);

    // officially contains an empty (epsilon) alternative, but instead we're making this
    // rule optional wherever it is used.
    join_constraint =
                (t.ON > expr)
            |   (t.USING > column_list)
            ;
    DISSEQT_DEBUG_NODE( join_constraint);

    column_list =
            '(' > column_name%',' > ')'
    ;
    DISSEQT_DEBUG_NODE( column_list);

    with_clause =
            omit[t.WITH] > (matches[t.RECURSIVE] >> common_table_expression %',')
            ;
    DISSEQT_DEBUG_NODE( with_clause);

    common_table_expression =
            table_name >> -column_list
            >>  t.AS >> '(' >> select_stmt >> ')'
            ;
    DISSEQT_DEBUG_NODE( common_table_expression);

    compound_operator =
            t.UNION >> -t.ALL
            |   t.INTERSECT
            |   t.EXCEPT
            ;
    DISSEQT_DEBUG_NODE( compound_operator);

    ordering_term =
                expr
            >>  -(t.COLLATE > collation_name)
            >>  (omit[t.ASC] >> attr(Ascending)| omit[t.DESC] >> attr(Descending) | eps >> attr(Ascending))
            ;
    DISSEQT_DEBUG_NODE( ordering_term);

    update_stmt =
            -with_clause >> t.UPDATE > -weasel_clause > qualified_table_name
            >   t.SET >> (column_name >> '=' >> expr)%',' >> -(t.WHERE > expr)
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
                omit[t.OR] >
                    (
                        omit[t.ROLLBACK]  >> attr( InsertRollback)
                    |   omit[t.ABORT]     >> attr( InsertAbort)
                    |   omit[t.REPLACE]   >> attr( Replace)
                    |   omit[t.FAIL]      >> attr( InsertFail)
                    |   omit[t.IGNORE]    >> attr( InsertIgnore)
                    ) [_val = _1]
            |   eps >> attr( Insert)
    ;
    DISSEQT_DEBUG_NODE( weasel_clause);

    expr =
                or_operand              [_val = _1]
            >>  *( t.OR >> or_operand)  [_val = ph::bind( binexp, Or, _val, _1)]
            ;
    DISSEQT_DEBUG_NODE( expr);

    or_operand =
                and_operand                 [_val = _1]
            >>  *( t.AND >> and_operand)    [ph::bind( binexp, And, _val, _1)]
            ;
    DISSEQT_DEBUG_NODE( or_operand);

    and_operand =
                compare_operand                             [_val = _1]
            >>  *(
                        (comparison_operator >> compare_operand) [_val = ph::bind( binexp, _1, _val, _2)]
                    |   (opt_not >> t.IN >>  set_expression)     [_val = ph::bind( inexp, _val, _2, _1)]
                    |   (opt_not >> t.BETWEEN >> compare_operand >> t.AND >> compare_operand) [_val = ph::bind( ternaryexp, Between, _val, _2, _3, _1)]
                    |   (t.ISNULL)          [ _val = ph::bind( binexp, Equals,    _val, null{})]
                    |   t.NOTNULL           [ _val = ph::bind( binexp, NotEquals, _val, null{})]
                    |   t.NOT >> t.NULL_T   [ _val = ph::bind( binexp, NotEquals, _val, null{})]
               )
            ;
    DISSEQT_DEBUG_NODE( and_operand);


    set_expression =
                composite_table_name
            |   '(' >> (select_stmt | -(expr%',')) >> ')'
            ;

    comparison_operator =
                omit[t.EQ_OP]       >> attr( Equals)
            |   omit["="]           >> attr( Equals)
            |   omit[t.NEQ_OP]      >> attr( NotEquals)
            |   omit[t.IS >> t.NOT] >> attr( NotEquals)
            |   omit[t.IS]          >> attr( Equals)
            |   omit[t.LIKE]        >> attr( Like)
            |   omit[t.NOT >> t.LIKE] >> attr( NotLike)
            |   omit[t.GLOB]        >> attr( Glob)
            |   omit[t.MATCH]       >> attr( Match)
            |   omit[t.REGEXP]      >> attr( Regexp)
            ;
    DISSEQT_DEBUG_NODE( comparison_operator);

    compare_operand =
            ineq_operand >> *( ineq_operator >> ineq_operand)
            ;
    DISSEQT_DEBUG_NODE( compare_operand);

    ineq_operator =
                omit[t.LE_OP]   >> attr( LessEquals)
            |   omit[t.GE_OP]   >> attr( GreaterEquals)
            |   omit['<']       >> attr( Less)
            |   omit['>']       >> attr( Greater)
            ;

    DISSEQT_DEBUG_NODE( ineq_operator);

    ineq_operand =
            bitwise_operand >> *(bitwise_operator >> bitwise_operand)
            ;
    DISSEQT_DEBUG_NODE( ineq_operand);


    bitwise_operand =
                term [_val = _1]
            >>  *(additive_operator >> term)[ _val = ph::bind( binexp, _1, _val, _2)]
            ;
    DISSEQT_DEBUG_NODE( bitwise_operand);

    term =
                factor [_val = _1]
            >> *( multiplicative_operator >> factor) [_val = ph::bind( binexp, _1, _val, _2)]
            ;
    DISSEQT_DEBUG_NODE( term);

    factor =
                collate [_val = _1]
            >>  *( t.CONCAT_OP >> collate [_val = ph::bind( binexp, Concat, _val, _1)])
            ;
    DISSEQT_DEBUG_NODE( factor);

    collate =
                singular                        [_val = _1]
            >>  -(t.COLLATE >> collation_name)  [_val = ph::bind(collateexp, _val, _1)]
            ;
    DISSEQT_DEBUG_NODE( collate);

    singular =
                exists_expr
            |   case_when
            |   cast_expr
            |   literal_value
            |   bind_parameter
            |   function_call_expr
            |   composite_column_name
            |   '(' >> select_stmt >> ')'   // not in the syntax diagrams, but described in "Table Column Names".
            |   '(' >> expr >> ')'
            |   unary_expr
            ;
    DISSEQT_DEBUG_NODE( singular);

    function_call_expr =
                (function_name >> '(' > -( '*' >> attr(ast::star())| function_arguments) >> ')')
            ;
    DISSEQT_DEBUG_NODE( function_call_expr);

    opt_distinct =
                omit[t.DISTINCT] >> attr( true)
            |   attr( false)
            ;

    function_arguments =
                opt_distinct >> (expr%',')
            ;
    DISSEQT_DEBUG_NODE(function_arguments);

    cast_expr =
                (t.CAST > '(' > expr > t.AS > type_name > ')')
            ;
    DISSEQT_DEBUG_NODE( cast_expr);

    exists_expr =
                (omit[t.EXISTS] > '(' >> select_stmt >> ')')
            ;
    DISSEQT_DEBUG_NODE( exists_expr);

    case_when  =
                (t.CASE > -(expr) >> +(t.WHEN > expr >> t.THEN > expr) >> -(t.ELSE > expr) >> omit[t.END])
            ;
    DISSEQT_DEBUG_NODE( case_when);

    unary_expr =
            unary_operator >> singular
            ;
    DISSEQT_DEBUG_NODE( unary_expr);

    bitwise_operator =
                omit[t.SHLEFT_OP]   >> attr( ShiftLeft)
            |   omit[t.SHRIGHT_OP]  >> attr( ShiftRight)
            |   omit['&']           >> attr( BitAnd)
            |   omit['|']           >> attr( BitOr)
            ;
    DISSEQT_DEBUG_NODE( bitwise_operator);

    unary_operator =
                (omit['-']   >> attr( Minus))
            |   (omit['+']   >> attr( Plus))
            |   (omit['~']   >> attr( BitNot))
            |   (omit[t.NOT] >> attr( Not))
            ;
    DISSEQT_DEBUG_NODE( unary_operator);

    multiplicative_operator =
                (omit['*']   >> attr( Times))
            |   (omit['/']   >> attr( Divided))
            |   (omit['%']   >> attr( Modulo))
            ;
    DISSEQT_DEBUG_NODE( multiplicative_operator);

    additive_operator =
                (omit['-']   >> attr( Minus))
            |   (omit['+']   >> attr( Plus))
            ;
    DISSEQT_DEBUG_NODE( additive_operator);

    // optional NOT
    // The synthesized attribute is a boolean that is true iff "NOT" appeared in the input.
    opt_not =
                t.NOT >> attr( true)
            |   attr( false)
            ;
    DISSEQT_DEBUG_NODE( opt_not);

    signed_number =
                ('-' >> attr(true) | '+' >> attr( false) | eps >> attr(false))
            >>  numeric_literal
            ;
    DISSEQT_DEBUG_NODE( signed_number);

    composite_column_name =
                name         [ph::at_c<2>(_val) = _1]
            >> -('.' >> name [ph::at_c<1>(_val) = ph::at_c<2>(_val), ph::at_c<2>(_val) = _1] )
            >> -('.' >> name [ph::at_c<0>(_val) = ph::at_c<1>(_val), ph::at_c<1>(_val) = ph::at_c<2>(_val), ph::at_c<2>(_val) = _1] )
            ;
    DISSEQT_DEBUG_NODE( composite_column_name);

    bind_parameter =
            t.BIND_PARAMETER
            ;
    DISSEQT_DEBUG_NODE( bind_parameter);

    function_name = name.alias()
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

    // names can be bare identifiers, but also be quoted.
    name =
        (
            t.IDENTIFIER
        |   t.STRING
            // add all "non-specific" keywords, i.e. keywords that also may appear as identifiers.
            BOOST_PP_SEQ_FOR_EACH( DISSEQT_PARSER_KEYWORD_ALTERNATIVE, _, DISSEQT_NONSPECIFIC_KEYWORDS)
        )[ph::at_c<0>(_val) = _1]
        ;
    DISSEQT_DEBUG_NODE( name);

}

template<typename Iterator>
struct GrammarInstantiator
{
    typedef ParserTypes<Iterator> parser_types;
    static void GetGrammar( const typename ParserTypes<Iterator>::lexer_type &lexer)
    {
        (void)SqlGrammar<typename LexerTypes<Iterator>::iterator_type>(lexer);
    }
};

}

typedef disseqt::LexerTypes< std::string::const_iterator> InstantiatedLexerTypes;
typedef typename InstantiatedLexerTypes::iterator_type lexer_iterator;
typedef disseqt::Lexer<typename InstantiatedLexerTypes::base_lexer_type> LexerType;
template disseqt::SqlGrammar< lexer_iterator>::SqlGrammar( const LexerType &);
template class disseqt::GrammarInstantiator<std::string::const_iterator>;


