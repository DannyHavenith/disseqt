/*
 * disseqt_ast.h
 *
 *  Created on: Apr 17, 2015
 *      Author: danny
 */

/**
 * @brief This file defines the types that comprise the Abstract Syntax Tree (AST) returned by the disseqt parser
 */
#ifndef DISSEQT_AST_NAMES_H_
#define DISSEQT_AST_NAMES_H_
#include <string>
#include <boost/range/iterator_range.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/support.hpp>
#include <boost/spirit/include/support_unused.hpp>
#include <boost/fusion/adapted/struct.hpp>

namespace disseqt
{
    namespace ast
    {
        typedef boost::iterator_range<std::string::const_iterator> text_reference;
        using boost::optional;

        struct generic_name_tag {};

        /**
         * A name struct holds a single name as it appears in the source text. The tag type is used to distinguish
         * between different types of names, such as table names, function names etc.
         * During parsing, names are tagged with generic_name_tag, until it is known which name is being parsed.
         */
        template< typename TagType>
        struct text
        {
            typedef TagType tag_type;

            text( ) = default;

            template<typename OtherTag>
            text( const text<OtherTag> &other): value( other.value){}

            template<typename OtherTag>
            text<TagType>& operator=( const text<OtherTag> &other)
            {
                value = other.value;
                return *this;
            }

            text( const text_reference &ref)
            :value( ref) {}

            text *operator=( const text_reference &ref)
            {
                value = ref;
                return *this;
            }

            std::string to_string() const
            {
                return std::string( value.begin(), value.end());
            }

            text_reference value;
            boost::spirit::unused_type dummy;
        };

        typedef text<generic_name_tag> generic_name;

        struct function_name_tag {};
        typedef text<function_name_tag> function_name;

        struct basic_table_name_tag {};
        struct foreign_table_tag : basic_table_name_tag {};
        typedef text<foreign_table_tag> foreign_table;

        struct index_name_tag {};
        typedef text<index_name_tag> index_name;

        struct table_name_tag : basic_table_name_tag {};
        typedef text<table_name_tag> table_name;

        struct database_name_tag {};
        typedef text<database_name_tag> database_name;

        struct collation_name_tag {};
        typedef text<collation_name_tag> collation_name;

        struct table_alias_tag {};
        typedef text<table_alias_tag> table_alias;

        struct column_alias_tag {};
        typedef text<column_alias_tag> column_alias;

        struct column_name_tag {};
        typedef text<column_name_tag> column_name;

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
        (disseqt::ast::text)(TagType),
        (disseqt::ast::text_reference, value)
        (boost::spirit::unused_type, dummy)
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

#endif /* DISSEQT_AST_NAMES_H_ */
