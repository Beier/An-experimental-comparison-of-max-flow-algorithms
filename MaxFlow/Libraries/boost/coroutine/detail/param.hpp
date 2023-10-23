
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_DETAIL_PARAM_H
#define BOOST_COROUTINES_DETAIL_PARAM_H

#include "../../config.hpp"
#include "../../mpl/eval_if.hpp"
#include "../../mpl/identity.hpp"
#include "../../mpl/or.hpp"
#include "../../type_traits/add_reference.hpp"
#include "../../type_traits/is_reference.hpp"
#include "../../type_traits/is_scalar.hpp"
#include "../../type_traits/is_stateless.hpp"

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

template< typename T >
struct param : 
    public mpl::eval_if<
        mpl::or_<
            is_scalar< T >,
            is_stateless< T >,
            is_reference< T >
        >,
        mpl::identity< T >,
        add_reference< T >
    >
{};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_COROUTINES_DETAIL_PARAM_H
