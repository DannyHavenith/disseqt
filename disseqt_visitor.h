/*
 * disseqt_visitor.h

 */
#ifndef DISSEQT_VISITOR_H_
#define DISSEQT_VISITOR_H_
#include <boost/variant.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/spirit/include/support.hpp>


#define DISSEQT_VISITOR_IMPL_TERMINAL( terminal_)   \
        void operator()( const terminal_ &node)     \
        {                                           \
            Derived().Visit( node);                 \
        };                                          \
        /**/

namespace disseqt {

    /**
     * Generic visitor implementation.
     *
     * This implementation does nothing more than visit each node in the AST.
     *
     * Derived classes can implement overrides of operator()() for specific types
     * and leave the other types to this class.
     */
    template< typename DerivedType>
    class AstVisitor : public boost::static_visitor<>
    {
        DerivedType &Derived()
        {
            return static_cast<const DerivedType &>( *this);
        }

        /// default implementation assumes that a node is a boost::fusion sequence and
        /// will visit each element of the sequence.
        template< typename NodeType>
        void operator()( NodeType &node)
        {
            if (Derived().Visit(node))
            {
                boost::fusion::for_each( node, std::ref( Derived()));
            }
        }

        template< typename NodeType>
        void operator()(const boost::recursive_wrapper<NodeType> &node)
        {
            Derived()(node.get());
        };

        template< typename Element>
        void operator()( const std::vector< Element> &node)
        {
            if (Derived().Visit( node))
            {
                std::for_each( node.begin(), node.end(), std::ref( Derived()));
            }
        }

        template< typename... VarNodes>
        void operator()( const boost::variant<VarNodes...> &node)
        {
            if (Derived().Visit( node))
            {
                boost::apply_visitor( std::ref( Derived()), node);
            }
        }

        template< typename NodeType>
        void operator()( const boost::optional< NodeType> &node)
        {
            if (node)
            {
                Derived()( node);
            }
        }

        // terminals of the syntax tree.

        template< typename TagType>
        void operator()( const ast::text<TagType> &node)
        {
            Derived().Visit( node);
        }

        DISSEQT_VISITOR_IMPL_TERMINAL( ast::star);
        DISSEQT_VISITOR_IMPL_TERMINAL( bool);
        DISSEQT_VISITOR_IMPL_TERMINAL( ast::default_values);
        DISSEQT_VISITOR_IMPL_TERMINAL( boost::spirit::qi::unused_type);
        DISSEQT_VISITOR_IMPL_TERMINAL( boost::iterator_range< std::string::const_iterator>);
        DISSEQT_VISITOR_IMPL_TERMINAL( ast::null);
        DISSEQT_VISITOR_IMPL_TERMINAL( ast::all);
        DISSEQT_VISITOR_IMPL_TERMINAL( ast::distinct);
        DISSEQT_VISITOR_IMPL_TERMINAL( ast::operator_type);

        template< typename Node>
        bool Visit( Node &)
        {
            return true;
        }
    };
}

#endif /* DISSEQT_VISITOR_H_ */
