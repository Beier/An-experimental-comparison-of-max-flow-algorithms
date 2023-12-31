
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_DETAIL_HOLDER_H
#define BOOST_COROUTINES_DETAIL_HOLDER_H

#include "../../assert.hpp"
#include "../../config.hpp"
#include "../../optional.hpp"

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

template< typename Data >
struct holder
{
    context::fcontext_t *   ctx;
    optional< Data >        data;
    bool                    force_unwind;

    holder( context::fcontext_t * ctx_) :
        ctx( ctx_), data(), force_unwind( false)
    { BOOST_ASSERT( ctx); }

    holder( context::fcontext_t * ctx_, Data data_) :
        ctx( ctx_), data( data_), force_unwind( false)
    { BOOST_ASSERT( ctx); }

    holder( context::fcontext_t * ctx_, bool force_unwind_) :
        ctx( ctx_), data(), force_unwind( force_unwind_)
    {
        BOOST_ASSERT( ctx);
        BOOST_ASSERT( force_unwind);
    }

    holder( holder const& other) :
        ctx( other.ctx), data( other.data),
        force_unwind( other.force_unwind)
    {}

    holder & operator=( holder const& other)
    {
        if ( this == & other) return * this;
        ctx = other.ctx;
        data = other.data;
        force_unwind = other.force_unwind;
        return * this;
    }
};

template<>
struct holder< void >
{
    context::fcontext_t *   ctx;
    bool                    force_unwind;

    holder( context::fcontext_t * ctx_, bool force_unwind_ = false) :
        ctx( ctx_), force_unwind( force_unwind_)
    { BOOST_ASSERT( ctx); }

    holder( holder const& other) :
        ctx( other.ctx), force_unwind( other.force_unwind)
    {}

    holder & operator=( holder const& other)
    {
        if ( this == & other) return * this;
        ctx = other.ctx;
        force_unwind = other.force_unwind;
        return * this;
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_COROUTINES_DETAIL_HOLDER_H
