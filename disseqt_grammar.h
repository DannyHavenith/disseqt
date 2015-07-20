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
    struct SqlGrammar : qi::grammar<Iterator, ast::sql_stmt_list(), Skipper>
    {
        template< typename Tokens>
        SqlGrammar( const Tokens &t);

        typedef qi::rule<Iterator, Skipper> rule;

        // not using a type alias because it needs to work on gcc 4.6
        template< typename Value>
        struct Rule
        {
            typedef qi::rule< Iterator, Value(), Skipper> t;
        };

        rule column_constraint;
        rule literal_value;
        rule foreign_key_clause;
        rule table_constraint;
        rule indexed_column;
        rule compound_operator;
        rule bind_parameter;

        typename Rule<ast::sql_stmt_list    >::t sql_stmt_list;
        typename Rule<ast::sql_stmt         >::t sql_stmt;
        typename Rule<ast::statement        >::t statement;
        typename Rule<ast::explain_stmt     >::t explain_stmt;
        typename Rule<ast::AlternateAction  >::t conflict_clause;
        typename Rule<ast::composite_table_name >::t composite_table_name;
        typename Rule<ast::composite_column_name>::t composite_column_name;
        typename Rule<ast::function_arguments   >::t function_arguments;
        typename Rule<ast::case_expression  >::t case_when;
        typename Rule<ast::select_statement >::t select_stmt;
        typename Rule<ast::function_name    >::t function_name;
        typename Rule<ast::foreign_table    >::t foreign_table;
        typename Rule<ast::index_name       >::t index_name;
        typename Rule<ast::table_name       >::t table_name;
        typename Rule<ast::database_name    >::t database_name;
        typename Rule<ast::collation_name   >::t collation_name;
        typename Rule<ast::column_name      >::t column_name;
        typename Rule<ast::table_alias      >::t table_alias;
        typename Rule<ast::column_alias     >::t column_alias;
        typename Rule<ast::generic_name     >::t name;
        typename Rule<bool                  >::t opt_not;
        typename Rule<bool                  >::t opt_distinct;
        typename Rule<ast::unary_op         >::t unary_expr;
        typename Rule<ast::expression       >::t singular;
        typename Rule<ast::operator_type    >::t unary_operator;
        typename Rule<ast::operator_type    >::t multiplicative_operator;
        typename Rule<ast::operator_type    >::t additive_operator;
        typename Rule<ast::operator_type    >::t bitwise_operator;
        typename Rule<ast::operator_type    >::t ineq_operator;
        typename Rule<ast::operator_type    >::t comparison_operator;
        typename Rule<ast::exists           >::t exists_expr;
        typename Rule<ast::type_name        >::t type_name;
        typename Rule<ast::cast             >::t cast_expr;
        typename Rule<ast::function_call    >::t function_call_expr;
        typename Rule<ast::expression       >::t expr;
        typename Rule<ast::numeric_literal  >::t numeric_literal;
        typename Rule<ast::string_literal   >::t string_literal;
        typename Rule<ast::blob_literal     >::t blob_literal;
        typename Rule<ast::expression       >::t or_operand;
        typename Rule<ast::expression       >::t and_operand;
        typename Rule<ast::expression       >::t compare_operand;
        typename Rule<ast::expression       >::t ineq_operand;
        typename Rule<ast::expression       >::t bitwise_operand;
        typename Rule<ast::expression       >::t term;
        typename Rule<ast::expression       >::t factor;
        typename Rule<ast::expression       >::t collate;
        typename Rule<ast::signed_number    >::t signed_number;
        typename Rule<ast::set_expression   >::t set_expression;
        typename Rule<ast::values_clause    >::t values_clause;
        typename Rule<ast::compound_select  >::t compound_select;
        typename Rule<ast::with_clause      >::t with_clause;
        typename Rule<ast::common_table_expression>::t common_table_expression;
        typename Rule<ast::column_list      >::t column_list;
        typename Rule<ast::expression_alias >::t expression_alias;
        typename Rule<ast::select_phrase    >::t select_core;
        typename Rule<ast::result_column    >::t result_column;
        typename Rule<ast::join_constraint  >::t join_constraint;
        typename Rule<ast::join_operator    >::t join_operator;
        typename Rule<ast::index_clause     >::t index_clause;
        typename Rule<ast::table_or_subquery>::t table_or_subquery;
        typename Rule<ast::join_clause      >::t join_clause;
        typename Rule<ast::join_expression  >::t join_expression;
        typename Rule<ast::ordering_term    >::t ordering_term;
        typename Rule<ast::order_by_clause  >::t order_by_clause;
        typename Rule<ast::limit_clause     >::t limit_clause;
        typename Rule<ast::AlternateAction  >::t weasel_clause;
        typename Rule<ast::AlternateAction  >::t insert_type;
        typename Rule<ast::insert_values    >::t insert_values;
        typename Rule<ast::insert_stmt      >::t insert_stmt;
        typename Rule<ast::create_table_stmt>::t create_table_stmt;
        typename Rule<ast::column_def       >::t column_def;
        typename Rule<ast::update_stmt      >::t update_stmt;
        typename Rule<ast::qualified_table_name>::t qualified_table_name;
        typename Rule<ast::update_limited_clause>::t update_limited_clause;
        typename Rule<ast::column_assignment>::t column_assignment;
        typename Rule<ast::column_assignments>::t column_assignments;

};
}
#endif /* DISSEQT_GRAMMAR_H_ */
