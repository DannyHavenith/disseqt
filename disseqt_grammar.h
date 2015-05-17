/*
 * parser.h
 *
 *  Created on: Apr 16, 2015
 *      Author: danny
 */

#ifndef DISSEQT_GRAMMAR_H_
#define DISSEQT_GRAMMAR_H_
#include <boost/spirit/include/qi.hpp>
#include "disseqt_ast_names.h"
#include "disseqt_ast_expressions.h"
#include "disseqt_ast_statements.h"

namespace disseqt
{

    namespace qi = boost::spirit::qi;

    template<typename Iterator>
    struct ParserTypes
    {
        typedef LexerTypes<Iterator>                     Lex;
        typedef Lexer< typename Lex::base_lexer_type>    lexer_type;
    };


    template< typename Iterator, typename Skipper = qi::rule<Iterator>>
    struct SqlGrammar : qi::grammar<Iterator, Skipper>
    {
        template< typename Tokens>
        SqlGrammar( const Tokens &t);

        typedef qi::rule<Iterator, Skipper> rule;

        // not using a type alias because it needs to work on gcc 4.6
        template< typename Value, typename Locals = qi::locals<>>
        struct Rule
        {
            typedef qi::rule< Iterator, Value(), Locals, Skipper> t;
        };

        rule insert_stmt;
        rule sql_stmt_list;
        rule sql_stmt;
        rule explain_stmt;
        rule stmt;
        rule create_table_stmt;
        rule column_def;
        rule column_constraint;
        rule literal_value;
        rule conflict_clause;
        rule foreign_key_clause;
        rule table_constraint;
        rule indexed_column;
        rule order_by_clause;
        rule limit_clause;
        rule select_phrase;
        rule result_column;
        rule table_or_subquery;
        rule table_clause;
        rule index_clause;
        rule join_clause;
        rule join_operator;
        rule join_constraint;
        rule compound_operator;
        rule ordering_term;
        rule update_stmt;
        rule qualified_table_name;
        rule update_limited_clause;
        rule weasel_clause;
        rule bind_parameter;
        rule dot;
        rule star;

        typename Rule<ast::composite_table_name >::t composite_table_name;
        typename Rule<ast::composite_column_name>::t composite_column_name;
        typename Rule<ast::function_arguments   >::t function_arguments;
        typename Rule<ast::case_expression      >::t case_when;
        typename Rule<ast::select_statement     >::t select_stmt;
        typename Rule<ast::function_name >::t function_name;
        typename Rule<ast::foreign_table >::t foreign_table;
        typename Rule<ast::index_name    >::t index_name;
        typename Rule<ast::table_name    >::t table_name;
        typename Rule<ast::database_name >::t database_name;
        typename Rule<ast::collation_name>::t collation_name;
        typename Rule<ast::column_name   >::t column_name;
        typename Rule<ast::table_alias   >::t table_alias;
        typename Rule<ast::column_alias  >::t column_alias;
        typename Rule<ast::generic_name  >::t name;
        typename Rule<bool               >::t opt_not;
        typename Rule<bool               >::t opt_distinct;
        typename Rule<ast::unary_op      >::t unary_expr;
        typename Rule<ast::expression    >::t singular;
        typename Rule<ast::operator_type >::t unary_operator;
        typename Rule<ast::operator_type >::t multiplicative_operator;
        typename Rule<ast::operator_type >::t additive_operator;
        typename Rule<ast::operator_type >::t bitwise_operator;
        typename Rule<ast::operator_type >::t ineq_operator;
        typename Rule<ast::operator_type >::t comparison_operator;
        typename Rule<ast::exists        >::t exists_expr;
        typename Rule<ast::type_name     >::t type_name;
        typename Rule<ast::cast          >::t cast_expr;
        typename Rule<ast::function_call >::t function_call_expr;
        typename Rule<ast::expression    >::t expr;
        typename Rule<ast::numeric_literal>::t numeric_literal;
        typename Rule<ast::string_literal>::t string_literal;
        typename Rule<ast::blob_literal  >::t blob_literal;
        typename Rule<ast::expression    >::t or_operand;
        typename Rule<ast::expression    >::t and_operand;
        typename Rule<ast::expression    >::t compare_operand;
        typename Rule<ast::expression    >::t ineq_operand;
        typename Rule<ast::expression    >::t bitwise_operand;
        typename Rule<ast::expression    >::t term;
        typename Rule<ast::expression    >::t factor;
        typename Rule<ast::expression    >::t collate;
        typename Rule<ast::signed_number >::t signed_number;
        typename Rule<ast::set_expression>::t set_expression;
        typename Rule<ast::values_clause        >::t values_clause;
        typename Rule<ast::compound_select>::t compound_select;
        typename Rule<ast::with_clause   >::t with_clause;
        typename Rule<ast::common_table_expression>::t common_table_expression;
        typename Rule<ast::column_list   >::t column_list;

    };
}
#endif /* DISSEQT_GRAMMAR_H_ */
