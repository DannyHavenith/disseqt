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
#include <vector>
#include <boost/fusion/adapted/struct.hpp>

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
            Collate,
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
        struct case_op;
        struct function_call;

        typedef boost::variant<
                boost::recursive_wrapper<ternary_op>,
                boost::recursive_wrapper<binary_op>,
                boost::recursive_wrapper<unary_op>,
                boost::recursive_wrapper<case_op>,
                boost::recursive_wrapper<function_call>
                >
                expression;

        struct unary_op
        {
            operator_type op;
            expression    e1;
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

        struct case_op
        {
            expression            case_expression;
            std::vector< when_op> when_expressions;
        };

        struct star {};
        struct function_arguments
        {
            bool distinct;
            std::vector< expression> arguments;
        };

        struct function_call
        {
            function_name function;
            boost::variant< star, function_arguments> arguments;
        };
    }
}

BOOST_FUSION_ADAPT_STRUCT(
       disseqt::ast::unary_op,
       (disseqt::ast::operator_type, op)
       (disseqt::ast::expression,   e1)
    )
#endif /* DISSEQT_AST_EXPRESSIONS_H_ */
