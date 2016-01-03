/*
 * disseqt_ast_print.h
 *
 *  Created on: Jul 19, 2015
 *      Author: danny
 */

#ifndef DISSEQT_AST_PRINT_H_
#define DISSEQT_AST_PRINT_H_
#include <boost/variant.hpp>


namespace disseqt {
namespace ast {

    template< typename ElementType>
    std::ostream &print( std::ostream &output, const std::vector<ElementType> &node);
    std::ostream &print( std::ostream &output, bool val);
    std::ostream &print( std::ostream &output, const boost::iterator_range< std::string::const_iterator> &keyword);
    std::ostream &print( std::ostream &output, const boost::spirit::qi::unused_type &);
    template< typename NodeType>
    std::ostream &print( std::ostream &output, const boost::recursive_wrapper<NodeType> &node);

    class NodePrinter : public boost::static_visitor<>
    {
    public:
        NodePrinter( std::ostream& strm)
        :m_strm( strm) {}

        template< typename NodeType>
        void operator()( const NodeType &node) const
        {
            print( m_strm, node);
            m_strm << ' ';
        }

    private:
        std::ostream &m_strm;
    };

    /**
     * Generic AST node printer. This assumes that the node is some
     * fusion sequence.
     */
    template< typename NodeType>
    std::ostream &print( std::ostream &output, const NodeType &node)
    {
        output << get_node_name(node) << '{';
        boost::fusion::for_each( node, NodePrinter{output});
        output << '}';
        return output;
    }

    std::ostream &print( std::ostream &output, const star &)
    {
        return output << '*';
    }

    std::ostream &print( std::ostream &output, bool val)
    {
        return output << std::boolalpha << val;
    }

    template< typename NodeType>
    std::ostream &print( std::ostream &output, const boost::recursive_wrapper<NodeType> &node)
    {
        return print( output, node.get());
    }

    template< typename ElementType>
    std::ostream &print( std::ostream &output, const std::vector<ElementType> &node)
    {
        output << get_node_name(node) << '[';
        std::for_each( node.begin(), node.end(), NodePrinter{output});
        output << ']';
        return output;
    }

    std::ostream &print( std::ostream &output, const default_values &)
    {
        return output << "{defaults}";
    }

    std::ostream &print( std::ostream &output, const boost::spirit::qi::unused_type &)
    {
        return output;
    }

    std::ostream &print( std::ostream &output, const boost::iterator_range< std::string::const_iterator> &keyword)
    {
        return output << '*' << std::string( keyword.begin(), keyword.end()) << '*';
    }

    template< typename NodeType>
    std::ostream &print( std::ostream &output, const boost::optional<NodeType> &node)
    {
        if (node) return print( output, *node);
        return output;
    }

    template< typename... VarNodes>
    std::ostream &print( std::ostream &output, const boost::variant<VarNodes...> &node)
    {
        boost::apply_visitor( NodePrinter{output}, node);
        return output;
    }

    std::ostream &print( std::ostream &output, const expression &)
    {
        output << "<expression>";
        return output;
    }

    template <typename T>
    const char *get_node_name( const T &)
    {
        return "#";
    }


#define ENUMVAL_TO_STRING( val_) case val_: return output << #val_;

    std::ostream &print( std::ostream &output, JoinType value)
    {
        switch( value)
        {
          ENUMVAL_TO_STRING( Inner)
          ENUMVAL_TO_STRING( Cross)
          ENUMVAL_TO_STRING( Left)
          ENUMVAL_TO_STRING( Right)
        default: return output << "Unknown enum value";
        }
    }
    std::ostream &print( std::ostream &output, OrderType value)
    {
        switch( value)
        {
            ENUMVAL_TO_STRING( Ascending)
            ENUMVAL_TO_STRING( Descending)
        default: return output << "Unknown enum value";
        }
    }

    std::ostream &print( std::ostream &output, AlternateAction value)
    {
        switch( value)
        {
            ENUMVAL_TO_STRING( NoAlternate)
            ENUMVAL_TO_STRING( Rollback)
            ENUMVAL_TO_STRING( Abort)
            ENUMVAL_TO_STRING( Fail)
            ENUMVAL_TO_STRING( Ignore)
            ENUMVAL_TO_STRING( Replace)
        default: return output << "Unknown enum value";
       }
    }
#undef ENUMVAL_TO_STRING

#define DECLARE_NODE_NAME( type_) const char *get_node_name( const type_ &) { return #type_;}
    // statements
    DECLARE_NODE_NAME( expression_alias)
    DECLARE_NODE_NAME( all_of_table)
    DECLARE_NODE_NAME( column_list)
    DECLARE_NODE_NAME( JoinType)
    DECLARE_NODE_NAME( AlternateAction)
    DECLARE_NODE_NAME( OrderType)
    DECLARE_NODE_NAME( ordering_term)
    DECLARE_NODE_NAME( order_by_clause)
    DECLARE_NODE_NAME( join_operator)
    DECLARE_NODE_NAME( index_clause)
    DECLARE_NODE_NAME( table_or_subquery)
    DECLARE_NODE_NAME( join_expression)
    DECLARE_NODE_NAME( join_clause)
    DECLARE_NODE_NAME( select_phrase)
    DECLARE_NODE_NAME( values_clause)
    DECLARE_NODE_NAME( default_values)
    DECLARE_NODE_NAME( common_table_expression)
    DECLARE_NODE_NAME(limit_clause)
    DECLARE_NODE_NAME( compound_select)
    DECLARE_NODE_NAME( with_clause)
    DECLARE_NODE_NAME( select_statement)
    DECLARE_NODE_NAME( insert_stmt)
    DECLARE_NODE_NAME( column_def)
    DECLARE_NODE_NAME( table_spec)
    DECLARE_NODE_NAME( table_def)
    DECLARE_NODE_NAME( create_table_stmt)
    DECLARE_NODE_NAME( update_limited_clause)
    DECLARE_NODE_NAME( qualified_table_name)
    DECLARE_NODE_NAME( column_assignment)
    DECLARE_NODE_NAME( update_stmt)
    DECLARE_NODE_NAME( statement)
    DECLARE_NODE_NAME( explain_stmt)
    DECLARE_NODE_NAME( sql_stmt)
    DECLARE_NODE_NAME( sql_stmt_list)
    DECLARE_NODE_NAME( result_columns)

    // names
    DECLARE_NODE_NAME( composite_column_name)
    DECLARE_NODE_NAME( composite_table_name)
    DECLARE_NODE_NAME( generic_name_tag)
    DECLARE_NODE_NAME( function_name)
    DECLARE_NODE_NAME( foreign_table)
    DECLARE_NODE_NAME( index_name)
    DECLARE_NODE_NAME( table_name)
    DECLARE_NODE_NAME( database_name)
    DECLARE_NODE_NAME( collation_name)
    DECLARE_NODE_NAME( table_alias)
    DECLARE_NODE_NAME( column_alias)
    DECLARE_NODE_NAME( column_name)



#undef DECLARE_NODE_NAME

}
}


#endif /* DISSEQT_AST_PRINT_H_ */
