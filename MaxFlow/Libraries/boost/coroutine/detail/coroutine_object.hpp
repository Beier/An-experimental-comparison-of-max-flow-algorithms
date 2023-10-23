
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_DETAIL_COROUTINE_OBJECT_H
#define BOOST_COROUTINES_DETAIL_COROUTINE_OBJECT_H

#include "../../assert.hpp"
#include "../../config.hpp"
#include "../../cstdint.hpp"
#include "../../exception_ptr.hpp"
#include "../../move/move.hpp"
#include "../../ref.hpp"
#include "../../tuple/tuple.hpp"
#include "../../type_traits/function_traits.hpp"

#include "../../coroutine/attributes.hpp"
#include "../../coroutine/detail/arg.hpp"
#include "../../coroutine/detail/config.hpp"
#include "../../coroutine/detail/coroutine_base.hpp"
#include "../../coroutine/detail/exceptions.hpp"
#include "../../coroutine/detail/flags.hpp"
#include "../../coroutine/detail/holder.hpp"
#include "../../coroutine/detail/param.hpp"
#include "../../coroutine/flags.hpp"

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

template< typename Coroutine >
void trampoline1( intptr_t vp)
{
    BOOST_ASSERT( vp);

    reinterpret_cast< Coroutine * >( vp)->run();
}

template< typename Coroutine, typename Arg >
void trampoline2( intptr_t vp)
{
    BOOST_ASSERT( vp);

    tuple< Coroutine *, Arg > * tpl(
        reinterpret_cast< tuple< Coroutine *, Arg > * >( vp) );
    Coroutine * coro( get< 0 >( * tpl) );
    Arg arg( get< 1 >( * tpl) );

    coro->run( arg);
}

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller,
    typename Result, int arity
>
class coroutine_object;

#include "../../coroutine/detail/coroutine_object_void_0.ipp"
#include "../../coroutine/detail/coroutine_object_void_1.ipp"
#include "../../coroutine/detail/coroutine_object_void_arity.ipp"
#include "../../coroutine/detail/coroutine_object_result_0.ipp"
#include "../../coroutine/detail/coroutine_object_result_1.ipp"
#include "../../coroutine/detail/coroutine_object_result_arity.ipp"

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_COROUTINES_DETAIL_COROUTINE_OBJECT_H
