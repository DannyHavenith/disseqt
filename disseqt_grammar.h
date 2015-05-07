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
        rule signed_number;
        rule factor;
        rule bind_parameter;

        typename Rule<ast::composite_table_name >::t composite_table_name;
        typename Rule<ast::composite_column_name>::t composite_column_name;
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
        typename Rule<bool>::t                opt_not;

        rule comparison_rhs;

    };
}
#endif /* DISSEQT_GRAMMAR_H_ */
