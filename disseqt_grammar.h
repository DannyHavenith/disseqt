/*
 * parser.h
 *
 *  Created on: Apr 16, 2015
 *      Author: danny
 */

#ifndef DISSEQT_GRAMMAR_H_
#define DISSEQT_GRAMMAR_H_
#include <boost/spirit/include/qi.hpp>

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
        rule signed_number;
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
        rule name;
    };
}
#endif /* DISSEQT_GRAMMAR_H_ */
