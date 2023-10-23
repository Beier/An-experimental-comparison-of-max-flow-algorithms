/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2009 Francois Barel

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_EXTRACT_PARAM_AUGUST_08_2009_0848AM)
#define BOOST_SPIRIT_EXTRACT_PARAM_AUGUST_08_2009_0848AM

#if defined(_MSC_VER)
#pragma once
#endif

#include "../../../../spirit/home/support/meta_compiler.hpp"
#include "../../../../spirit/home/support/nonterminal/locals.hpp"
#include "../../../../spirit/home/support/unused.hpp"
#include "../../../../spirit/home/support/common_terminals.hpp"

#include "../../../../function_types/is_function.hpp"
#include "../../../../function_types/parameter_types.hpp"
#include "../../../../function_types/result_type.hpp"
#include "../../../../fusion/include/as_list.hpp"
#include "../../../../fusion/include/as_vector.hpp"
#include "../../../../mpl/deref.hpp"
#include "../../../../mpl/end.hpp"
#include "../../../../mpl/eval_if.hpp"
#include "../../../../mpl/find_if.hpp"
#include "../../../../mpl/identity.hpp"
#include "../../../../mpl/if.hpp"
#include "../../../../mpl/placeholders.hpp"
#include "../../../../type_traits/is_same.hpp"

namespace boost { namespace spirit { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    // Helpers to extract params (locals, attributes, ...) from nonterminal
    // template arguments
    ///////////////////////////////////////////////////////////////////////////
    template <typename Types, typename Pred, typename Default>
    struct extract_param
    {
        typedef typename mpl::find_if<Types, Pred>::type pos;

        typedef typename
            mpl::eval_if<
                is_same<pos, typename mpl::end<Types>::type>
              , mpl::identity<Default>
              , mpl::deref<pos>
            >::type
        type;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Types>
    struct extract_locals
      : fusion::result_of::as_vector<
            typename extract_param<
                Types
              , is_locals<mpl::_>
              , locals<>
            >::type
        >
    {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename Domain, typename Types>
    struct extract_component
      : spirit::result_of::compile<
            Domain
          , typename extract_param<
                Types
              , traits::matches<Domain, mpl::_>
              , unused_type
            >::type
        >
    {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename Types>
    struct extract_sig
      : extract_param<
            Types
          , function_types::is_function<mpl::_>
          , void()
        >
    {};

    template <typename Sig>
    struct attr_from_sig
    {
        typedef typename function_types::result_type<Sig>::type attr;

        typedef typename
            mpl::if_<
                is_same<attr, void>
              , unused_type
              , attr
            >::type
        type;
    };

    template <typename Sig>
    struct params_from_sig
    {
        typedef typename function_types::parameter_types<Sig>::type params;

        typedef typename fusion::result_of::as_list<params>::type type;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Types>
    struct extract_encoding
      : extract_param<
            Types
          , is_char_encoding<mpl::_>
          , unused_type
        >
    {};
}}}

#endif
