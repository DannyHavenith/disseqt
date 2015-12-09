/*
 * disseqt_ast_queries.h
 *
 *  Created on: Dec 8, 2015
 *      Author: danny
 */

#ifndef DISSEQT_DISSEQT_AST_QUERIES_H_
#define DISSEQT_DISSEQT_AST_QUERIES_H_
#include "disseqt_visitor.h"

namespace disseqt
{


    template< typename VisitorBuilder>
    class QueryBuilder
    {
    public:
        template< typename VisitorType, typename NodeType>
        static VisitorType apply( NodeType &node, VisitorType v = VisitorType{})
        {
            VisitorBuilder::create(std::ref(v))( node);
            return v;
        }

        /// within creates a Query builder that builds visitors that
        /// travel the tree and only start deploying the original visitor
        template<typename... NodeTypes>
        using within = QueryBuilder<BuildTopDown<BuildDropper<VisitorBuilder, NodeTypes...>>>;
    };


    /// for_every creates visitors that traverse the tree and apply an embedded
    /// visitor at every node that is of one of the specified types.
    template< typename... NodeTypes>
    using for_every = QueryBuilder<BuildTopDown<BuildDropper<BuildIdentity, NodeTypes...>>>;
    using for_all = QueryBuilder<BuildTopDown<BuildIdentity>>;

}




#endif /* DISSEQT_DISSEQT_AST_QUERIES_H_ */
