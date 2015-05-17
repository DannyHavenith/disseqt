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
        struct compound_operator {};

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                expression_alias,
                (expression, e)
                (column_alias, alias)
                )


        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                all_of_table,
                (table_name, table)
            )

        struct where_clause {};
        struct group_by_clause {};

        struct join_operator {};
        struct table_or_subquery {};
        struct join_constraint {};

        typedef boost::optional<index_name> index_clause;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                join_clause,
                (join_operator,                     op)
                (table_or_subquery,                 left)
                (table_or_subquery,                 right)
                (boost::optional<join_constraint>,  constraint)
                )

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                table_clause,
                (composite_table_name,          table)
                (boost::optional<table_alias>,  alias)
                (boost::optional<index_clause>, index)
                )

        typedef boost::variant<
                join_clause,
                std::vector<table_or_subquery>
            > from_clause;

        typedef boost::variant<
                star,
                all_of_table,
                expression_alias
                >
            result_column;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                select_phrase,
                (std::vector<result_column>,         columns)
                (boost::optional< from_clause>,      from)
                (boost::optional< where_clause>,     where)
                (boost::optional< group_by_clause>,  group_by)
        )

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                values_clause,
                (std::vector< expression>, v)
        )

        typedef boost::variant<values_clause, select_phrase> value_phrase;

        struct select_statement;

        typedef std::vector<column_name> column_list;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                common_table_expression,
                (table_name, name)
                (boost::optional<column_list>, columns)
                (boost::recursive_wrapper<select_statement>, select)
                )


        BOOST_FUSION_DEFINE_STRUCT_INLINE( order_by_clause, )
        BOOST_FUSION_DEFINE_STRUCT_INLINE(limit_clause, )

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

        typedef boost::variant<
                select_statement,
                update,
                create_table,
                insert
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



#endif /* DISSEQT_AST_STATEMENTS_H_ */
