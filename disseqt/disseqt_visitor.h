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
#include <boost/mpl/set.hpp>
#include <boost/mpl/contains.hpp>

#include <type_traits> // for std::enable_if
#include <functional> // for std::ref

namespace disseqt {

    /**
     * Generic top-down visitor.
     *
     * This partial visitor implementation implements an iteration strategy on the
     * AST; specifically a top-down, depth first strategy.
     *
     * This visitor takes an embedded visitor object that will be applied to each node
     * in the AST. The embedded visitor can prohibit further recursion down the tree
     * by returning false from its operator()
     *
     * As is customary in C++ (at least in boost.variant) the actual "Visit()" function
     * is called operator().
     *
     * This class derives from boost::static_visitor so that it can be applied
     * to variants.
     *
     */
    template< typename ActualVisitor>
    class TopDownVisitor : public boost::static_visitor<bool>
    {
    public:

        TopDownVisitor( const ActualVisitor &actual = ActualVisitor{})
        :m_actual{ actual}
        {}

        /// By default, for fusion sequences, the behaviour is to visit the object first
        /// and then to descent into its constituents (top down visitation).
        template< typename NodeType>
        typename std::enable_if<
            boost::fusion::traits::is_sequence<NodeType>::value,
            bool>::type
        operator()( const NodeType &node)
        {
            if (m_actual(node))
            {
                boost::fusion::for_each( node, std::ref( *this));
            }
            return false;
        }

        /// By default, for anything that is not a fusion sequence, we only visit
        /// the type and leave it at that. This is done to easily cover all terminals
        /// of the AST.
        template< typename NodeType>
        typename std::enable_if<
            not boost::fusion::traits::is_sequence<NodeType>::value,
            bool
        >::type
        operator()( const NodeType &node)
        {
            m_actual( node);
            return false;
        }

        /// Recursive wrappers get silently unwrapped and the wrapped type is visited.
        template< typename NodeType>
        bool operator()(const boost::recursive_wrapper<NodeType> &node)
        {
            (*this)(node.get());
            return false;
        };

        /// Vectors will be visited and then each element will be visited.
        template< typename Element>
        bool operator()( const std::vector< Element> &node)
        {
            if (m_actual( node))
            {
                std::for_each( node.begin(), node.end(), std::ref( *this));
            }
            return false;
        }

        /// Variants get visited as a variant first, but then get visited again
        /// after their actual type is deduced.
        template< typename... VarNodes>
        bool operator()( const boost::variant<VarNodes...> &node)
        {
            if (m_actual( node))
            {
                boost::apply_visitor( std::ref( *this), node);
            }
            return false;
        }

        template< typename NodeType>
        bool operator()( const boost::optional< NodeType> &node)
        {
            if (node)
            {
                (*this)( *node);
            }
            return false;
        }

    private:
        // taking the actualvisitor by value.
        ActualVisitor m_actual;
    };

    struct BuildIdentity
    {
        template< typename Visitor>
        using Composite = Visitor;

        template< typename Visitor>
        static Visitor create( Visitor v)
        {
            return v;
        }

    };

    template< typename NextBuilder>
    struct BuildTopDown
    {
        template< typename Visitor>
        using Composite = TopDownVisitor< typename NextBuilder::template Composite<Visitor>>;

        template< typename Visitor>
        static Composite<Visitor> create( Visitor v)
        {
            return Composite<Visitor>( NextBuilder::create( v));
        }
    };

    template<typename Visitor, typename NodeType>
    Visitor &VisitTopDown( Visitor &v, NodeType &node)
    {
        TopDownVisitor<Visitor &> td( v);
        td( node);
        return v;
    }


    /**
     * Visitor that applies an embedded visitor for specific types.
     *
     * This class is a visitor that can be applied to any type and do nothing,
     * but if the object that is offered is of a type that is in the list of
     * applicable types, it will apply an embedded visitor.
     */
    template< typename Payload, typename... ApplicableTypes>
    class VisitorDropper
    {
    public:
        using TypesSet = boost::mpl::set<ApplicableTypes...>;

        VisitorDropper( const Payload &payload)
        :m_payload{ payload}
        {}

        template< typename T>
        typename std::enable_if<
            boost::mpl::contains< TypesSet, T>::value,
            bool
        >::type
        operator()( T &node)
        {
            return m_payload( node);
        }

        template< typename T>
        typename std::enable_if<
            boost::mpl::contains< TypesSet, T>::value,
            bool
        >::type
        operator()( const T &node)
        {
            return m_payload( node);
        }

        template< typename T>
        typename std::enable_if<
             not boost::mpl::contains< TypesSet, T>::value,
             bool
         >::type operator()( const T&)
        {
            return true;
        }

    private:
        Payload m_payload;
    };

    template< typename NextBuilder, typename... ApplicableTypes>
    struct BuildDropper
    {
        template< typename Visitor>
        using Composite = VisitorDropper< typename NextBuilder:: template Composite<Visitor>, ApplicableTypes...>;

        template< typename Visitor>
        static Composite<Visitor> create( Visitor v)
        {
            return Composite<Visitor>( NextBuilder::create( v));
        }

    };
}

#endif /* DISSEQT_VISITOR_H_ */
