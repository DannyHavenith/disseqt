/*
 * disseqt_visitor.h

 */
#ifndef DISSEQT_VISITOR_H_
#define DISSEQT_VISITOR_H_
#include <boost/variant.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/spirit/include/support.hpp>

#include <type_traits> // for std::enable_if

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
     * Visitation is split into two parts: operator()() and Visit().
     * operator()() is expected to call Visit() on an object and then recurse into the sub-objects
     * Visit() is expected to only visit an object (do whatever the object wants to do).
     *
     *
     */
    template< typename DerivedType>
    class AstVisitor : public boost::static_visitor<>
    {
    public:

        /// By default, for fusion sequences, the behaviour is to visit the object first
        /// and then to descent into its constituents (top down visitation).
        template< typename NodeType>
        typename std::enable_if<boost::fusion::traits::is_sequence<NodeType>::value>::type
        operator()( const NodeType &node)
        {
            if (Derived().Visit(node))
            {
                boost::fusion::for_each( node, std::ref( Derived()));
            }
        }

        /// By default, for anything that is not a fusion sequence, we only visit
        /// the type and leave it at that. This is done to easily cover all terminals
        /// of the AST.
        template< typename NodeType>
        typename std::enable_if<not boost::fusion::traits::is_sequence<NodeType>::value>::type
        operator()( const NodeType &node)
        {
            Derived().Visit( node);
        }

        /// Recursive wrappers get silently unwrapped and the wrapped type is visited.
        template< typename NodeType>
        void operator()(const boost::recursive_wrapper<NodeType> &node)
        {
            Derived()(node.get());
        };

        /// Vectors will be visited and then each element will be visited.
        template< typename Element>
        void operator()( const std::vector< Element> &node)
        {
            if (Derived().Visit( node))
            {
                std::for_each( node.begin(), node.end(), std::ref( Derived()));
            }
        }

        /// Variants get visited as a variant first, but then get visited again
        /// after their actual type is deduced.
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
                Derived()( *node);
            }
        }

        /// The default implementation of the Visit() function does nothing.
        template< typename Node>
        bool Visit( Node &)
        {
            return true;
        }

    private:
        DerivedType &Derived()
        {
            return static_cast<DerivedType &>( *this);
        }

        DerivedType &Derived() const
        {
            return static_cast<const DerivedType &>( *this);
        }

    };
}

#endif /* DISSEQT_VISITOR_H_ */
