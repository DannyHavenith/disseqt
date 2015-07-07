/*
 * disseqt_ast_statements.h
 *
 *  Created on: May 7, 2015
 *      Author: danny
 */

#ifndef DISSEQT_AST_STATEMENTS_H_
#define DISSEQT_AST_STATEMENTS_H_
#include <boost/fusion/adapted/struct/define_struct_inline.hpp>
#include <boost/variant.hpp>

namespace disseqt {
    namespace ast {

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                expression_alias,
                (expression, e)
                (column_alias, alias)
                )


        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                all_of_table,
                (table_name, table)
            )


        typedef std::vector<column_name> column_list;

        enum JoinType
        {
            Inner,
            Cross,
            Left,
            Right
        };

        enum InsertType
        {
            Insert,
            InsertRollback,
            InsertAbort,
            InsertReplace,
            InsertFail,
            InsertIgnore,
            Replace
        };

        enum OrderType
        {
            Ascending,
            Descending
        };

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                ordering_term,
                (expression,                        expr)
                (boost::optional<collation_name>,   collation)
                (OrderType,                         order))

        typedef std::vector< ordering_term>
                    order_by_clause;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                join_operator,
                (bool,     natural)
                (JoinType, type)
                )

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

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                table_or_subquery,
                (table_or_select_t, table_or_select)
                (boost::optional<table_alias>, alias)
                (index_clause, index)
                )


        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                join_expression,
                (join_operator,                     op)
                (table_or_subquery,                 right)
                (boost::optional<join_constraint>,  constraint)
                )

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                join_clause,
                (table_or_subquery,         first)
                (std::vector<join_expression>,  joined)
                )

        typedef boost::variant<
                star,
                table_name,
                expression_alias
                >
            result_column;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                select_phrase,
                (std::vector<result_column>,         columns)
                (boost::optional< join_clause>,      from)
                (boost::optional< expression>,       where)
                (boost::optional< std::vector<expression>>,  group_by)
                (boost::optional< expression>,       having)
        )

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                values_clause,
                (std::vector< expression>, v)
        )


        struct default_values {};


        struct select_statement;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                common_table_expression,
                (table_name, name)
                (boost::optional<column_list>, columns)
                (boost::recursive_wrapper<select_statement>, select)
                )


        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                limit_clause,
                (expression, offset)
                (expression, limit)
                )

        typedef boost::variant<values_clause, select_phrase> value_phrase;
        typedef boost::variant<values_clause, select_statement, default_values> insert_values;
        typedef std::vector<value_phrase> compound_select;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                with_clause,
                (bool, recursive)
                (std::vector<common_table_expression>, expressions)
                )

        struct update {};
        struct create_table {};
        struct insert {};

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
            select_statement,
            (boost::optional< with_clause>,       with)
            (std::vector<value_phrase>,           val)
            (boost::optional<order_by_clause>,    order_by)
            (boost::optional<limit_clause>,       limit)
        )

        // for some reason, the DEFINE_STRUCT_INLINE macro
        // had some misalignment issues, so we're doing the hard-core ADAPT_STRUCT thing
        // here.
        struct insert_stmt
        {
            boost::optional< with_clause>   with;
            InsertType                      insert_type;
            composite_table_name            table;
            boost::optional<column_list>    columns;
            insert_values                   values;
        };

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                column_def,
                (column_name,                   column)
                (boost::optional<type_name>,    type)
        )

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                table_spec,
                (std::vector<column_def>,   columns)
                (bool,                      without_identifier)
                )

        typedef boost::variant<
                select_statement,
                table_spec> table_def;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
            create_table_stmt,
            (bool,                  temporary)
            (bool,                  if_not_exist)
            (composite_table_name,  table_name)
            (table_def,             definition)
        )

//        BOOST_FUSION_DEFINE_STRUCT_INLINE(
//                update_stmt,
//                (boost::optional< with_clause>, with)
//                (boost::optional<weasel_clause>, weasel_clause)
//
//                )

        typedef boost::variant<
                select_statement,
                update,
                create_table,
                insert_stmt
                > statement;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                explain,
                (statement, s)
            )

        template< typename T>
        typename std::enable_if< std::is_class<T>::value, std::ostream &>::type
        operator<<( std::ostream &strm, const T &)
        {
            return strm << typeid(T).name();
        }
}
}

BOOST_FUSION_ADAPT_STRUCT(
        disseqt::ast::insert_stmt,
        (boost::optional< disseqt::ast::with_clause>,   with)
        (disseqt::ast::InsertType,                      insert_type)
        (disseqt::ast::composite_table_name,            table)
        (boost::optional<disseqt::ast::column_list>,    columns)
        (disseqt::ast::insert_values,                   values)
    )



#endif /* DISSEQT_AST_STATEMENTS_H_ */
