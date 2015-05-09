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

        struct from_clause {};
        struct where_clause {};
        struct group_by_clause {};


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
                values,
                (std::vector< expression>, v)
        )

        typedef boost::variant<values, select_phrase> value_phrase;

        struct select_statement;

        typedef std::vector<column_name> column_list;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                common_table_expression,
                (table_name, name)
                (boost::optional<column_list>, columns)
                (boost::recursive_wrapper<select_statement>, select)
                )

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
                with_clause,
                (bool, recursive)
                (std::vector<common_table_expression>, expressions)
                )

        struct order_by_clause {};
        struct limit_clause {};

        typedef std::vector<value_phrase> compound_select;

        BOOST_FUSION_DEFINE_STRUCT_INLINE(
            select_statement,
            (boost::optional< with_clause>,       with)
            (std::vector<value_phrase>,           val)
            (boost::optional<order_by_clause>,    order_by)
            (boost::optional<limit_clause>,       limit)
        )

        struct update {};
        struct create_table {};
        struct insert {};

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
}
}



#endif /* DISSEQT_AST_STATEMENTS_H_ */
