/*
 * disseqt_ast_expressions.h
 *
 *  Created on: May 7, 2015
 *      Author: danny.havenith
 */

#ifndef DISSEQT_AST_EXPRESSIONS_H_
#define DISSEQT_AST_EXPRESSIONS_H_

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/optional.hpp>
#include <vector>

#include "disseqt_ast_names.h"

namespace disseqt {
    namespace ast {

        enum operator_type {
            Plus,
            Minus,
            Times,
            Divided,
            Modulo,
            And,
            Or,
            Xor,
            Less,
            LessEquals,
            Greater,
            GreaterEquals,
            Equals,
            NotEquals,
            ShiftLeft,
            ShiftRight,
            BitAnd,
            BitOr,
            Like,
            NotLike,
            IsNull,
            Exists,
            In,
            Between,
            Glob,
            Match,
            Regexp,
            Concat,
            Not,
            BitNot
        };

        struct ternary_op;
        struct binary_op;
        struct unary_op;
        struct select_stmt;
        struct case_expression;
        struct function_call;
        struct select_statement;
        struct cast;
        struct in;
        struct collate;

        struct exists
        {
            boost::recursive_wrapper<select_statement> select;
        };

        struct null {};
        struct star {};
        struct all {};
        struct distinct {};

        struct literal_tag {};
        struct string_literal_tag :  public literal_tag {};
        struct numeric_literal_tag : public literal_tag {};
        struct blob_literal_tag :    public literal_tag {};

        typedef text<string_literal_tag>    string_literal;
        typedef text<blob_literal_tag>      blob_literal;
        typedef text<numeric_literal_tag>   numeric_literal;

        typedef boost::variant<
                string_literal,
                numeric_literal,
                blob_literal,
                composite_column_name,
                exists,
                boost::recursive_wrapper<ternary_op>,
                boost::recursive_wrapper<binary_op>,
                boost::recursive_wrapper<unary_op>,
                boost::recursive_wrapper<case_expression>,
                boost::recursive_wrapper<function_call>,
                boost::recursive_wrapper<cast>,
                boost::recursive_wrapper<in>,
                boost::recursive_wrapper<select_statement>,
                boost::recursive_wrapper<collate>,
                null
                >
                expression;

        typedef boost::variant<
                boost::recursive_wrapper<composite_table_name>,
                std::vector<expression>,
                boost::recursive_wrapper< select_statement>
                >
                set_expression;

        struct in
        {
            expression      e;
            set_expression  set;
        };

        struct collate
        {
            expression      e;
            collation_name  name;
        };

        struct unary_op
        {
            operator_type op;
            expression    e1;
        };

        struct signed_number
        {
            bool            minus;
            numeric_literal literal;
        };

        struct binary_op
        {
            operator_type op;
            expression    e1;
            expression    e2;
        };

        struct ternary_op
        {
            operator_type op;
            expression  e1;
            expression  e2;
            expression  e3;
        };

        struct when_op
        {
            expression when;
            expression then;
        };

        struct case_expression
        {
            boost::optional<expression> case_expression;
            std::vector< when_op>       when_expressions;
            boost::optional<expression> else_expression;
        };

        struct function_arguments
        {
            bool distinct;
            std::vector< expression> arguments;
        };

        typedef boost::variant< star, function_arguments> function_args;

        struct function_call
        {
            function_name function;
            function_args arguments;
        };

        struct type_name
        {
            std::vector< generic_name> names;
        };

        struct cast
        {
            expression  expr;
            type_name   type;
        };

    }
}

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::type_name,
        (std::vector< disseqt::ast::generic_name>, names)
        )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::function_arguments,
        (bool, distinct)
        (std::vector<disseqt::ast::expression>, arguments)
    )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::function_call,
        (disseqt::ast::function_name, function)
        (disseqt::ast::function_args, arguments)
    )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::cast,
        (disseqt::ast::expression,  expr)
        (disseqt::ast::type_name,   type)
    )

BOOST_FUSION_ADAPT_STRUCT(
       disseqt::ast::unary_op,
       (disseqt::ast::operator_type,    op)
       (disseqt::ast::expression,       e1)
    )

BOOST_FUSION_ADAPT_STRUCT(
       disseqt::ast::binary_op,
       (disseqt::ast::operator_type,    op)
       (disseqt::ast::expression,       e1)
       (disseqt::ast::expression,       e2)
    )

BOOST_FUSION_ADAPT_STRUCT(
       disseqt::ast::ternary_op,
       (disseqt::ast::operator_type,    op)
       (disseqt::ast::expression,       e1)
       (disseqt::ast::expression,       e2)
       (disseqt::ast::expression,       e3)
    )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::exists,
        (boost::recursive_wrapper<disseqt::ast::select_statement>, select)
        )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::case_expression,
        (boost::optional<disseqt::ast::expression>,   case_expression)
        (std::vector<disseqt::ast::when_op>,          when_expressions)
        (boost::optional<disseqt::ast::expression>,   else_expression)
        )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::when_op,
        (disseqt::ast::expression, when)
        (disseqt::ast::expression, then)
        )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::signed_number,
        (bool, minus)
        (disseqt::ast::numeric_literal, literal)
    )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::in,
        (disseqt::ast::expression,        e)
        (disseqt::ast::set_expression,    set)
    )
#endif /* DISSEQT_AST_EXPRESSIONS_H_ */
