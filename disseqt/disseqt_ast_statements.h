/*
 * disseqt_ast_statements.h
 *
 *  Created on: May 7, 2015
 *      Author: danny
 */

#ifndef DISSEQT_AST_STATEMENTS_H_
#define DISSEQT_AST_STATEMENTS_H_
#include "disseqt_ast_expressions.h"
#include <boost/fusion/adapted/struct/define_struct_inline.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <vector>

namespace disseqt {
    namespace ast {

        struct expression_alias
        {
            expression e;
            column_alias alias;
        };


        struct all_of_table
        {
            table_name table;
        };


        typedef std::vector<column_name> column_list;

        enum JoinType
        {
            Inner,
            Cross,
            Left,
            Right
        };

        enum AlternateAction
        {
            NoAlternate,
            Rollback,
            Abort,
            Fail,
            Ignore,
            Replace
        };

        enum OrderType
        {
            Ascending,
            Descending
        };

        struct ordering_term
        {
            expression                        expr;
            boost::optional<collation_name>   collation;
            OrderType                         order;
        };

        struct join_operator
        {
            bool     natural;
            JoinType type;
        };

        typedef std::vector< ordering_term>
                    order_by_clause;


        typedef boost::variant<
                expression,
                column_list
                >
                join_constraint;

        typedef boost::variant<
                composite_table_name,
                boost::recursive_wrapper< select_statement>
                >
                table_or_select_t;

        typedef boost::optional<index_name>
                    index_clause;

        struct table_or_subquery
        {
            table_or_select_t table_or_select;
            boost::optional<table_alias> alias;
            index_clause index;
        };


        struct join_expression
        {
            join_operator                     op;
            table_or_subquery                 right;
            boost::optional<join_constraint>  constraint;
        };

        struct join_clause
        {
            table_or_subquery         first;
            std::vector<join_expression>  joined;
        };

        struct values_clause
        {
            std::vector< expression> v;
        };

        typedef boost::variant<
                star,
                table_name,
                expression_alias
                >
            result_column;

        typedef std::vector<result_column> result_columns;

        struct select_phrase
        {
            result_columns                     columns;
            boost::optional< join_clause>      from;
            boost::optional< expression>       where;
            boost::optional< std::vector<expression>>  group_by;
            boost::optional< expression>       having;
        };



        struct default_values {};
        struct select_statement;

        struct common_table_expression
        {
            table_name name;
            boost::optional<column_list> columns;
            boost::recursive_wrapper<select_statement> select;
        };


        struct limit_clause
        {
            expression offset;
            expression limit;
        };

        typedef boost::variant<values_clause, select_phrase> value_phrase;
        typedef std::vector<value_phrase> compound_select;

        struct with_clause
        {
            bool recursive;
            std::vector<common_table_expression> expressions;
        };




        struct select_statement
        {
            boost::optional< with_clause>       with;
            std::vector<value_phrase>           val;
            boost::optional<order_by_clause>    order_by;
            boost::optional<limit_clause>       limit;
        };

        typedef boost::variant<values_clause, select_statement, default_values> insert_values;

        // for some reason, the DEFINE_STRUCT_INLINE macro
        // had some misalignment issues, so we're doing the hard-core ADAPT_STRUCT thing
        // here.
        struct insert_stmt
        {
            boost::optional< with_clause>   with;
            AlternateAction                 insert_type;
            composite_table_name            table;
            boost::optional<column_list>    columns;
            insert_values                   values;
        };

        struct column_def
        {
            column_name                   column;
            boost::optional<type_name>    type;
        };

        struct table_spec
        {
            std::vector<column_def>   columns;
            bool                      without_identifier;
        };

        typedef boost::variant<
                select_statement,
                table_spec> table_def;

        struct create_table_stmt
        {
            bool                  temporary;
            bool                  if_not_exist;
            composite_table_name  table_name;
            table_def             definition;
        };

        struct update_limited_clause
        {
            boost::optional<order_by_clause>  order_by;
            limit_clause                      limit;
        };

        struct qualified_table_name
        {
            composite_table_name name;
            boost::optional<index_clause> index;
        };

        struct column_assignment
        {
            column_name column;
            expression value;
        };


        typedef std::vector<column_assignment> column_assignments;
        struct update_stmt
        {
            boost::optional< with_clause> with;
            AlternateAction weasel_clause;
            qualified_table_name table;
            column_assignments assignments;
            boost::optional<expression> where;
            boost::optional<update_limited_clause> limit;
        };


        typedef boost::variant<
                select_statement,
                update_stmt,
                create_table_stmt,
                insert_stmt
                > statement;

        struct explain_stmt
        {
            statement   stmt;
        };

        typedef boost::variant<
                statement,
                explain_stmt>
                sql_stmt;

        typedef std::vector< sql_stmt>
            sql_stmt_list;


}
}

// We're using ADAPT_STRUCT instead of DEFINE_STRUCT_INLINE because the
// latter introduces a templated constructor which messes up the SFINAE
// that is used by ApplyWhereApplicable<>.

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::column_assignment,
        (disseqt::ast::column_name, column)
        (disseqt::ast::expression, value)
)

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::update_stmt,
        (boost::optional< disseqt::ast::with_clause>, with)
        (disseqt::ast::AlternateAction, weasel_clause)
        (disseqt::ast::qualified_table_name, table)
        (disseqt::ast::column_assignments, assignments)
        (boost::optional<disseqt::ast::expression>, where)
        (boost::optional<disseqt::ast::update_limited_clause>, limit)
        )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::insert_stmt,
        (boost::optional< disseqt::ast::with_clause>,   with)
        (disseqt::ast::AlternateAction,                      insert_type)
        (disseqt::ast::composite_table_name,            table)
        (boost::optional<disseqt::ast::column_list>,    columns)
        (disseqt::ast::insert_values,                   values)
    )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::select_phrase,
        (disseqt::ast::result_columns,                     columns)
        (boost::optional< disseqt::ast::join_clause>,      from)
        (boost::optional< disseqt::ast::expression>,       where)
        (boost::optional< std::vector<disseqt::ast::expression>>,  group_by)
        (boost::optional< disseqt::ast::expression>,       having)
    )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::qualified_table_name,
        (disseqt::ast::composite_table_name,          name)
        (boost::optional<disseqt::ast::index_clause>, index)
    )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::with_clause,
        (bool, recursive)
        (std::vector<disseqt::ast::common_table_expression>, expressions)
        )

BOOST_FUSION_ADAPT_STRUCT(
    disseqt::ast::select_statement,
    (boost::optional< disseqt::ast::with_clause>,       with)
    (std::vector<disseqt::ast::value_phrase>,           val)
    (boost::optional<disseqt::ast::order_by_clause>,    order_by)
    (boost::optional<disseqt::ast::limit_clause>,       limit)
)

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::column_def,
        (disseqt::ast::column_name,                   column)
        (boost::optional<disseqt::ast::type_name>,    type)
)

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::table_spec,
        (std::vector<disseqt::ast::column_def>,   columns)
        (bool,                      without_identifier)
        )

BOOST_FUSION_ADAPT_STRUCT(
    disseqt::ast::create_table_stmt,
    (bool,                  temporary)
    (bool,                  if_not_exist)
    (disseqt::ast::composite_table_name,  table_name)
    (disseqt::ast::table_def,             definition)
)

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::update_limited_clause,
        (boost::optional<disseqt::ast::order_by_clause>,  order_by)
        (disseqt::ast::limit_clause,                      limit)
        )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::common_table_expression,
        (disseqt::ast::table_name, name)
        (boost::optional<disseqt::ast::column_list>, columns)
        (boost::recursive_wrapper<disseqt::ast::select_statement>, select)
        )


BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::limit_clause,
        (disseqt::ast::expression, offset)
        (disseqt::ast::expression, limit)
        )


BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::table_or_subquery,
        (disseqt::ast::table_or_select_t, table_or_select)
        (boost::optional<disseqt::ast::table_alias>, alias)
        (disseqt::ast::index_clause, index)
        )


BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::join_expression,
        (disseqt::ast::join_operator,                     op)
        (disseqt::ast::table_or_subquery,                 right)
        (boost::optional<disseqt::ast::join_constraint>,  constraint)
        )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::join_clause,
        (disseqt::ast::table_or_subquery,         first)
        (std::vector<disseqt::ast::join_expression>,  joined)
        )

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::values_clause,
        (std::vector< disseqt::ast::expression>, v)
)

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::ordering_term,
        (disseqt::ast::expression,                        expr)
        (boost::optional<disseqt::ast::collation_name>,   collation)
        (disseqt::ast::OrderType,                         order))

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::join_operator,
        (bool,     natural)
        (disseqt::ast::JoinType, type)
        )

BOOST_FUSION_ADAPT_STRUCT(
         disseqt::ast::expression_alias,
         (disseqt::ast::expression, e)
         (disseqt::ast::column_alias, alias)
         )

 BOOST_FUSION_ADAPT_STRUCT(
         disseqt::ast::all_of_table,
         (disseqt::ast::table_name, table)
     )

 BOOST_FUSION_ADAPT_STRUCT(
         disseqt::ast::explain_stmt,
         (disseqt::ast::statement, stmt)
         )
#endif /* DISSEQT_AST_STATEMENTS_H_ */
