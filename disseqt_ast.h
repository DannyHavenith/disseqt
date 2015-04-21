/*
 * disseqt_ast.h
 *
 *  Created on: Apr 17, 2015
 *      Author: danny
 */

/**
 * @brief This file defines the types that comprise the Abstract Syntax Tree (AST) returned by the disseqt parser
 */
#ifndef DISSEQT_AST_H_
#define DISSEQT_AST_H_
#include <boost/range/iterator_range.hpp>
#include <boost/optional.hpp>
#include <boost/variant/variant.hpp>
#include <boost/fusion/adapted/struct.hpp>

namespace disseqt
{
    namespace ast
    {
        typedef boost::iterator_range<std::string::const_iterator> value_type;
        using boost::optional;
        using boost::variant;

        struct generic_name_tag {};
        template< typename TagType>
        struct name
        {
            typedef TagType tag_type;
            name( ) = default;
            template<typename OtherTag>
            name( const name<OtherTag> &other): value( other.value){}
            template<typename OtherTag>
            name<TagType>& operator=( const name<OtherTag> &other)
            {
                value = other.value;
                return *this;
            }

            value_type value;
            boost::spirit::qi::unused_type dummy;
        };


        typedef name<generic_name_tag> generic_name;

        struct function_name_tag {};
        typedef name<function_name_tag> function_name;

        struct foreign_table_tag {};
        typedef name<foreign_table_tag> foreign_table;

        struct index_name_tag {};
        typedef name<index_name_tag> index_name;

        struct table_name_tag {};
        typedef name<table_name_tag> table_name;

        struct database_name_tag {};
        typedef name<database_name_tag> database_name;

        struct collation_name_tag {};
        typedef name<collation_name_tag> collation_name;

        struct table_alias_tag {};
        typedef name<table_alias_tag> table_alias;

        struct column_alias_tag {};
        typedef name<column_alias_tag> column_alias;

        struct column_name_tag {};
        typedef name<column_name_tag> column_name;

        struct composite_table_name
        {
            optional< database_name> database;
            table_name               table;
        };

        struct composite_column_name
        {
            optional< database_name> database;
            optional< table_name>    table;
            column_name              column;
        };
    }
}

BOOST_FUSION_ADAPT_TPL_STRUCT(
        (TagType),
        (disseqt::ast::name)(TagType),
        (disseqt::ast::value_type, value)
        (boost::spirit::qi::unused_type, dummy)
        )

BOOST_FUSION_ADAPT_STRUCT(
            disseqt::ast::composite_table_name,
            ( boost::optional<disseqt::ast::database_name>, database)
            ( disseqt::ast::table_name, table)
        )

BOOST_FUSION_ADAPT_STRUCT(
            disseqt::ast::composite_column_name,
            ( boost::optional<disseqt::ast::database_name>, database)
            ( boost::optional<disseqt::ast::table_name>, table)
            ( disseqt::ast::column_name, column)
        )

#endif /* DISSEQT_AST_H_ */
