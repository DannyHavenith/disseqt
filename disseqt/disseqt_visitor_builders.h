/*
 * disseqt_ast_queries.h
 *
 *  Created on: Dec 8, 2015
 *      Author: danny
 */

#ifndef DISSEQT_DISSEQT_VISITOR_BUILDERS_H_
#define DISSEQT_DISSEQT_VISITOR_BUILDERS_H_

#include "disseqt_visitors.h"
#include <utility> // for std::forward, std::declval

namespace disseqt
{

    /**
     * Visitor builders delegate construction of actual visitors to "constructors"
     *
     * At construction time, these constructors will always be given an "inner"
     * visitor to embed but nothing else.
     *
     * Some constructors therefore have to store additional constructor arguments
     * to be used when the visitor needs to be constructed.
     *
     * This default one however has none.
     */
    class DefaultConstructor
    {
    public:
        template< typename Visitor, typename InnerVisitor>
        static Visitor construct( InnerVisitor &&v)
        {
            return Visitor( std::forward<InnerVisitor>(v));
        }
    };

    /**
     * A constructor that stores one additional visitor constructor argument.
     */
    template< typename Arg>
    class SingleArgumentConstructor
    {
    public:
        SingleArgumentConstructor( Arg arg)
        :m_argument( arg)
        {}

        template< typename Visitor, typename InnerVisitor>
        Visitor construct( const InnerVisitor &v)
        {
            return Visitor( v, m_argument);
        }
    private:
        Arg m_argument;
    };

    /**
     * VisitorStorage acts as the innermost visitor builder. It does not
     * create any composite visitors, but simply returns a reference to
     * an internally stored visitor.
     */
    template< typename Visitor>
    class VisitorStorage {
    public:
        typedef Visitor InnerVisitor;

        /**
         * Simple wrapper around a reference to a visitor.
         *
         * This class allows an IdentityBuilder to hold a copy of a visitor
         * and give all derived visitors a reference to that inner visitor.
         */
        class VisitorReference {
        public:
            VisitorReference( Visitor &v)
            :m_visitor(v)
            {}

            // due to the way R is defined, SFINAE will prevent this operator
            // to be present if Visitor cannot be applied to an object of type
            // T.
            template<typename T, typename R =  decltype( (std::declval<Visitor &>())( std::declval<T>()))>
            R operator()(T &t)
            {
                return m_visitor( t);
            }

            Visitor &GetVisitor()
            {
                return m_visitor;
            }

        private:
            Visitor &m_visitor;
        };

        explicit VisitorStorage( Visitor visitor)
        :m_visitor(visitor)
        {
        }

        Visitor& create()
        {
            return m_visitor;
        }

        Visitor &get_inner()
        {
            return m_visitor;
        }

    private:

        Visitor m_visitor;
    };

    /**
     * VisitorBuilders are those objects that can create nested AST visitors,
     * using visitor adaptors such as TopDownVisitor or VisitorDropper.
     *
     * VisitorBuilders are themselves nested as well and will delegate building
     * of inner visitors to inner visitor builders.
     *
     * The member functions of visitor builders create yet other visitor builders.
     * Because each member function returns a VisitorBuilder with similar member
     * methods, the methods can be chained to create more complex visitors,
     * for example:
     *
     * @code
     * apply( my_visitor)            // this stores the first visitor
     * .in_every<select_statement>() // this creates a visitor builder
     * .within<insert_statement>()   // this wraps the first visitor builder
     * .in( some_ast);               // this creates the nested visitor and applies it.
     */
    template< typename PreviousBuilder, typename Visitor, typename VisitorConstructor = DefaultConstructor>
    class VisitorBuilder : private VisitorConstructor
    {
    public:
        typedef VisitorBuilder<PreviousBuilder, Visitor, VisitorConstructor> ThisType;
        typedef typename PreviousBuilder::InnerVisitor InnerVisitor;

        template< typename... Args>
        VisitorBuilder( const PreviousBuilder &previous, Args... args)
        :VisitorConstructor{args...},
         m_previous{previous}
        {}

        /**
         * Apply the inner visitor in every node for which the inner visitor has
         * a valid operator().
         */
        VisitorBuilder<ThisType, TopDownVisitor<ApplyWhereApplicable<Visitor>>>
        everywhere()
        {
            return {*this};
        }

        template<typename... NodeTypes>
        VisitorBuilder<ThisType, TopDownVisitor<VisitorDropper<Visitor, NodeTypes...>>>
        in_every()
        {
            return { *this};
        }

        template<typename... NodeTypes>
        VisitorBuilder<ThisType, TopDownVisitor<VisitorDropper<Visitor, NodeTypes...>>>
        within()
        {
            return { *this};
        }

        template< typename NodeType, typename MemberType>
        VisitorBuilder<
            ThisType,
            AtMemberDropper<Visitor, NodeType, MemberType>,
            SingleArgumentConstructor<MemberType NodeType::*>
            >
        if_at( MemberType NodeType::*member)
        {
            return {*this, member};
        }

        template< typename NodeType, typename MemberType>
        auto within(MemberType NodeType::*member) -> decltype( this->if_at( member).everywhere())
        {
            return if_at( member).everywhere();
        }

        InnerVisitor &get_inner()
        {
            return m_previous.get_inner();
        }

        Visitor create()
        {
            return VisitorConstructor::template construct<Visitor>( m_previous.create());
        }

        template<typename NodeType, typename... NodeTypes>
        InnerVisitor in( NodeType &node, NodeTypes &... nodes)
        {
            auto v = create();
            // create a visitor and apply it to the node
            Apply( v, node, nodes...);

            // return the inner visitor
            return get_inner();
        }

    private:

        template<typename V>
        void Apply( const V &)
        {
            // nop
        }

        template<typename V, typename NodeType, typename... NodeTypes>
        void Apply( typename VisitorStorage<V>::VisitorReference &v, NodeType &node, NodeTypes &... nodes)
        {
            Apply( v.GetVisitor(), nodes...);
        }

        template<typename V, typename NodeType, typename... NodeTypes>
        void Apply( V &v, NodeType &node, NodeTypes &... nodes)
        {
            v( node);
            Apply( v, nodes...);
        }

        PreviousBuilder m_previous;
    };

    /**
     * The apply function is used to create the first Visitor Builder in a possibly
     * nested chain of builders.
     *
     * @see VisitorBuilder
     */
    template< typename Visitor>
    VisitorBuilder<VisitorStorage<Visitor>, typename VisitorStorage<Visitor>::VisitorReference> apply( const Visitor &v = Visitor{})
    {
        return {VisitorStorage<Visitor>(v)};
    }

}




#endif /* DISSEQT_DISSEQT_VISITOR_BUILDERS_H_ */
