
#ifndef BOOST_MPL_TRANSFORM_HPP_INCLUDED
#define BOOST_MPL_TRANSFORM_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: transform.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include "../mpl/fold.hpp"
#include "../mpl/reverse_fold.hpp"
#include "../mpl/pair_view.hpp"
#include "../mpl/is_sequence.hpp"
#include "../mpl/eval_if.hpp"
#include "../mpl/lambda.hpp"
#include "../mpl/bind.hpp"
#include "../mpl/or.hpp"
#include "../mpl/not.hpp"
#include "../mpl/aux_/na.hpp"
#include "../mpl/aux_/inserter_algorithm.hpp"

namespace boost { namespace mpl {

namespace aux { 

template< 
      typename Seq
    , typename Op
    , typename In
    >
struct transform1_impl
    : fold< 
          Seq
        , typename In::state
        , bind2< typename lambda< typename In::operation >::type
            , _1
            , bind1< typename lambda<Op>::type, _2>
            > 
        >
{
};

template< 
      typename Seq
    , typename Op
    , typename In
    >
struct reverse_transform1_impl
    : reverse_fold< 
          Seq
        , typename In::state
        , bind2< typename lambda< typename In::operation >::type
            , _1
            , bind1< typename lambda<Op>::type, _2>
            > 
        >
{
};

template< 
      typename Seq1
    , typename Seq2
    , typename Op
    , typename In
    >
struct transform2_impl
    : fold< 
          pair_view<Seq1,Seq2>
        , typename In::state
        , bind2< typename lambda< typename In::operation >::type
            , _1
            , bind2<
                  typename lambda<Op>::type
                , bind1<first<>,_2>
                , bind1<second<>,_2>
                >
            > 
        >
{
};

template< 
      typename Seq1
    , typename Seq2
    , typename Op
    , typename In
    >
struct reverse_transform2_impl
    : reverse_fold< 
          pair_view<Seq1,Seq2>
        , typename In::state
        , bind2< typename lambda< typename In::operation >::type
            , _1
            , bind2< typename lambda< Op >::type
                , bind1<first<>,_2>
                , bind1<second<>,_2>
                >
            > 
        >
{
};

} // namespace aux 

BOOST_MPL_AUX_INSERTER_ALGORITHM_DEF(3, transform1)                    
BOOST_MPL_AUX_INSERTER_ALGORITHM_DEF(4, transform2)
    
#define AUX778076_TRANSFORM_DEF(name)                                   \
template<                                                               \
      typename BOOST_MPL_AUX_NA_PARAM(Seq1)                             \
    , typename BOOST_MPL_AUX_NA_PARAM(Seq2OrOperation)                  \
    , typename BOOST_MPL_AUX_NA_PARAM(OperationOrInserter)              \
    , typename BOOST_MPL_AUX_NA_PARAM(Inserter)                         \
    >                                                                   \
struct name                                                             \
{                                                                       \
    typedef typename eval_if<                                           \
          or_<                                                          \
              is_na<OperationOrInserter>                                \
            , is_lambda_expression< Seq2OrOperation >                   \
            , not_< is_sequence<Seq2OrOperation> >                      \
            >                                                           \
        , name##1<Seq1,Seq2OrOperation,OperationOrInserter>             \
        , name##2<Seq1,Seq2OrOperation,OperationOrInserter,Inserter>    \
        >::type type;                                                   \
};                                                                      \
BOOST_MPL_AUX_NA_SPEC(4, name)                                          \
/**/

AUX778076_TRANSFORM_DEF(transform)
AUX778076_TRANSFORM_DEF(reverse_transform)

#undef AUX778076_TRANSFORM_DEF

}}

#endif // BOOST_MPL_TRANSFORM_HPP_INCLUDED
