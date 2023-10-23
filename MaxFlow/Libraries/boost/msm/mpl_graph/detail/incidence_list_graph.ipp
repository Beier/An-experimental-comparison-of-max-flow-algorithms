// Copyright 2008-2010 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MSM_MPL_GRAPH_DETAIL_INCIDENCE_LIST_GRAPH_IPP_INCLUDED

#define BOOST_MSM_MPL_GRAPH_DETAIL_INCIDENCE_LIST_GRAPH_IPP_INCLUDED

// these metafunctions provide the metadata structures needed by the public interface 
// in mpl_graph.hpp

#include "../../../mpl/map.hpp"
#include "../../../mpl/vector.hpp"
#include "../../../mpl/copy.hpp"
#include "../../../mpl/vector.hpp"
#include "../../../mpl/next.hpp"
#include "../../../mpl/front.hpp"
#include "../../../mpl/back.hpp"
#include "../../../mpl/deref.hpp"
#include "../../../mpl/if.hpp"
#include "../../../mpl/size.hpp"
#include "../../../mpl/void.hpp"
#include "../../../mpl/erase_key.hpp"
#include "../../../mpl/has_key.hpp"
#include "../../../mpl/inserter.hpp"
#include "../../../mpl/back_inserter.hpp"
#include "../../../mpl/set.hpp"
#include "../../../mpl/insert.hpp"
#include "../../../mpl/transform.hpp"
#include "../../../mpl/pair.hpp"
#include "../../../mpl/size.hpp"
#include "../../../mpl/fold.hpp"
#include "../../../mpl/transform.hpp"
#include "../../../mpl/at.hpp"
#include "../../../mpl/push_back.hpp"
#include "../../../mpl/filter_view.hpp"
#include "../../../mpl/transform_view.hpp"
#include "../../../mpl/equal.hpp"
#include "../../../type_traits.hpp"


namespace boost {
namespace msm {
namespace mpl_graph {
namespace detail {

// tag to identify this graph implementation (not defined)
struct incidence_list_tag;
    
// clarifiers
template<typename EST> struct fetch_edge : 
    mpl::front<EST> {};
template<typename EST> struct fetch_source : 
    mpl::deref<typename mpl::next<typename mpl::begin<EST>::type>::type> {};
template<typename EST> struct fetch_target : 
    mpl::back<EST> {};

// Edge->Target map for an Source for out_*, adjacent_vertices
template<typename Source, typename ESTSequence>
struct produce_out_map<incidence_list_tag, Source, ESTSequence> :
    mpl::fold<typename mpl::filter_view<ESTSequence, boost::is_same<fetch_source<mpl::_1>,Source> >::type,
         mpl::map<>,
         mpl::insert<mpl::_1,mpl::pair<fetch_edge<mpl::_2>,fetch_target<mpl::_2> > > >
{};

// Edge->Source map for a Target for in_*, degree
template<typename Target, typename ESTSequence>
struct produce_in_map<incidence_list_tag, Target, ESTSequence> :
    mpl::fold<typename mpl::filter_view<ESTSequence, 
                                        boost::is_same<fetch_target<mpl::_1>,Target> >::type,
         mpl::map<>,
         mpl::insert<mpl::_1,mpl::pair<fetch_edge<mpl::_2>,fetch_source<mpl::_2> > > >

{};
// Edge->pair<Source,Target> map for source, target
template<typename ESTSequence>
struct produce_edge_st_map<incidence_list_tag, ESTSequence> :
    mpl::fold<ESTSequence,
         mpl::map<>,
         mpl::insert<mpl::_1,mpl::pair<fetch_edge<mpl::_2>,
                        mpl::pair<fetch_source<mpl::_2>, 
                             fetch_target<mpl::_2> > > > >
{};
// Vertex set for VertexListGraph
template<typename ESTSequence>
struct produce_vertex_set<incidence_list_tag, ESTSequence> :
    mpl::fold<ESTSequence,
         typename mpl::fold<ESTSequence,
                       mpl::set<>,
                       mpl::insert<mpl::_1,fetch_target<mpl::_2> >
                       >::type,
         mpl::insert<mpl::_1, fetch_source<mpl::_2> > >
{};
// Edge set for EdgeListGraph
template<typename ESTSequence>
struct produce_edge_set<incidence_list_tag, ESTSequence> :
    mpl::fold<ESTSequence,
        mpl::set<>,
        mpl::insert<mpl::_1,fetch_edge<mpl::_2> > >
{};
}
}
}
}

#endif // BOOST_MSM_MPL_GRAPH_DETAIL_INCIDENCE_LIST_GRAPH_IPP_INCLUDED
