/*=============================================================================
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_SEGMENTED_FOLD_UNTIL_HPP_INCLUDED)
#define BOOST_FUSION_SEGMENTED_FOLD_UNTIL_HPP_INCLUDED

#include "../../type_traits/is_const.hpp"
#include "../../utility/enable_if.hpp"
#include "../../fusion/support/detail/segmented_fold_until_impl.hpp"
#include "../../fusion/view/iterator_range.hpp"
#include "../../fusion/sequence/intrinsic/begin.hpp"
#include "../../fusion/sequence/intrinsic/end.hpp"
#include "../../fusion/sequence/intrinsic/empty.hpp"
#include "../../fusion/container/list/cons.hpp"

namespace boost { namespace fusion
{
    //auto segmented_fold_until(seq, state, fun)
    //{
    //  return first(segmented_fold_until_impl(seq, state, nil, fun));
    //}

    namespace result_of
    {
        template <typename Sequence, typename State, typename Fun>
        struct segmented_fold_until
        {
            typedef
                detail::segmented_fold_until_impl<
                    Sequence
                  , State
                  , fusion::nil
                  , Fun
                >
            filter;

            typedef
                typename filter::type
            type;
        };
    }

    template <typename Sequence, typename State, typename Fun>
    typename 
        lazy_disable_if<
            is_const<Sequence>
          , result_of::segmented_fold_until<Sequence, State, Fun>
        >::type
    segmented_fold_until(Sequence& seq, State const& state, Fun const& fun)
    {
        typedef
            typename result_of::segmented_fold_until<Sequence, State, Fun>::filter
        filter;
        
        return filter::call(seq, state, fusion::nil(), fun);
    }

    template <typename Sequence, typename State, typename Fun>
    typename result_of::segmented_fold_until<Sequence const, State, Fun>::type
    segmented_fold_until(Sequence const& seq, State const& state, Fun const& fun)
    {
        typedef
            typename result_of::segmented_fold_until<Sequence const, State, Fun>::filter
        filter;
        
        return filter::call(seq, state, fusion::nil(), fun);
    }
}}

#endif
