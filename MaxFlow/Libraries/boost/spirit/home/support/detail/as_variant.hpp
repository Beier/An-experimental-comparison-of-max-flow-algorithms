/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_PP_IS_ITERATING
#if !defined(BOOST_SPIRIT_AS_VARIANT_NOVEMBER_16_2007_0420PM)
#define BOOST_SPIRIT_AS_VARIANT_NOVEMBER_16_2007_0420PM

#include "../../../../preprocessor/iterate.hpp"
#include "../../../../preprocessor/repetition/enum_params.hpp"
#include "../../../../preprocessor/repetition/enum_binary_params.hpp"
#include "../../../../preprocessor/repetition/enum_params_with_a_default.hpp"
#include "../../../../preprocessor/repetition/repeat_from_to.hpp"
#include "../../../../variant/variant_fwd.hpp"
#include "../../../../fusion/include/size.hpp"
#include "../../../../fusion/include/begin.hpp"
#include "../../../../fusion/include/next.hpp"
#include "../../../../fusion/include/value_of.hpp"
#include "../../../../mpl/fold.hpp"
#include "../../../../mpl/vector.hpp"
#include "../../../../mpl/push_back.hpp"
#include "../../../../mpl/equal_to.hpp"
#include "../../../../mpl/contains.hpp"
#include "../../../../type_traits/is_same.hpp"

namespace boost { namespace spirit { namespace detail
{
    template <int size>
    struct as_variant_impl;

    template <>
    struct as_variant_impl<0>
    {
        template <typename Iterator>
        struct apply
        {
            typedef variant<> type;
        };
    };

#define BOOST_FUSION_NEXT_ITERATOR(z, n, data)                                  \
    typedef typename fusion::result_of::next<BOOST_PP_CAT(I, n)>::type          \
        BOOST_PP_CAT(I, BOOST_PP_INC(n));

#define BOOST_FUSION_NEXT_CALL_ITERATOR(z, n, data)                             \
    typename gen::BOOST_PP_CAT(I, BOOST_PP_INC(n))                              \
        BOOST_PP_CAT(i, BOOST_PP_INC(n)) = fusion::next(BOOST_PP_CAT(i, n));

#define BOOST_FUSION_VALUE_OF_ITERATOR(z, n, data)                              \
    typedef typename fusion::result_of::value_of<BOOST_PP_CAT(I, n)>::type      \
        BOOST_PP_CAT(T, n);

#define BOOST_PP_FILENAME_1 "../../../../spirit/home/support/detail/as_variant.hpp"
#define BOOST_PP_ITERATION_LIMITS (1, BOOST_VARIANT_LIMIT_TYPES)
#include BOOST_PP_ITERATE()

#undef BOOST_FUSION_NEXT_ITERATOR
#undef BOOST_FUSION_NEXT_CALL_ITERATOR
#undef BOOST_FUSION_VALUE_OF_ITERATOR

    template <typename Sequence>
    struct as_variant
    {
        // build a variant generator being able to generate a variant holding
        // all of the types as given in the typelist
        typedef typename
            detail::as_variant_impl<fusion::result_of::size<Sequence>::value>
        gen;

        // use this generator to create the actual variant
        typedef typename gen::template apply<
                typename fusion::result_of::begin<Sequence>::type
            >::type
        type;
    };
}}}

#endif
#else // defined(BOOST_PP_IS_ITERATING)
///////////////////////////////////////////////////////////////////////////////
//
//  Preprocessor vertical repetition code
//
///////////////////////////////////////////////////////////////////////////////

#define N BOOST_PP_ITERATION()

    template <>
    struct as_variant_impl<N>
    {
        template <typename I0>
        struct apply
        {
            BOOST_PP_REPEAT(N, BOOST_FUSION_NEXT_ITERATOR, _)
            BOOST_PP_REPEAT(N, BOOST_FUSION_VALUE_OF_ITERATOR, _)
            typedef variant<BOOST_PP_ENUM_PARAMS(N, T)> type;
        };
    };

#undef N
#endif // defined(BOOST_PP_IS_ITERATING)

