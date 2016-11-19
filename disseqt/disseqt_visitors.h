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
     * AST; specifically depth first pre-order strategy.
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
        }

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

        /// Optionals get the same treatment as variants: first the optional
        /// itself is visited and if the optional has a value, then that
        /// value will be visited.
        template< typename NodeType>
        bool operator()( const boost::optional< NodeType> &node)
        {
            if (m_actual( node) and node)
            {
                (*this)( *node);
            }
            return false;
        }

    private:
        // taking the actualvisitor by value.
        ActualVisitor m_actual;
    };

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

    /**
    * Visitor that applies an embedded visitor wherever applying
    * the embedded visitor would not be a compiler error
    *
    * This will apply a visitor for each object for which the visitor
    * has defined an operator()().
    */
    template< typename Payload>
    class ApplyWhereApplicable
    {
    public:
        ApplyWhereApplicable( const Payload &payload)
        :m_payload{ payload}
        {
        }

        template< typename Node>
        bool operator()( Node &&node)
        {
            return ApplyIfCallable( m_payload, std::forward<Node>( node));
        }
    private:
        Payload m_payload;

	template< typename Functor, typename... ArgTypes>
	static auto ApplyIfCallable( Functor &&f, ArgTypes &&... args)
		-> decltype( std::forward<Functor>(f)( std::forward<ArgTypes>( args)...))
	{
		return std::forward<Functor>(f)( std::forward<ArgTypes>(args)...);
	}

	static bool ApplyIfCallable( ...)
	{
		return true;
	}
    };

    /**
     * A Visitor that applies an embedded visitor at specific members of
     * specified types.
     *
     * Objects of this class can be applied to any type and do nothing, but
     * if objects of type 'Struct' are offered, an embedded visitor will
     * be applied to a single member or that struct.
     */
    template< typename Visitor, typename Struct, typename MemberType>
    class AtMemberDropper {
    public:
        AtMemberDropper( const Visitor &v, MemberType Struct::*member)
        :m_visitor( v),m_member( member)
        {}

        template< typename T>
        bool operator()( T &node)
        {
            return true;
        }

        bool operator()( Struct &s)
        {
            return m_visitor( s.*m_member);
        }

        bool operator()( const Struct &s)
        {
            return m_visitor( s.*m_member);
        }

    private:
        Visitor             m_visitor;
        MemberType Struct::*m_member;
    };

}

#endif /* DISSEQT_VISITOR_H_ */
