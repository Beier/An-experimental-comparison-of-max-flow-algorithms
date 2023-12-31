
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_DETAIL_COROUTINE_BASE_H
#define BOOST_COROUTINES_DETAIL_COROUTINE_BASE_H

#include "../../assert.hpp"
#include "../../config.hpp"
#include "../../context/fcontext.hpp"
#include "../../exception_ptr.hpp"
#include "../../intrusive_ptr.hpp"
#include "../../type_traits/function_traits.hpp"
#include "../../utility.hpp"

#include "../../coroutine/detail/config.hpp"
#include "../../coroutine/detail/coroutine_base_resume.hpp"
#include "../../coroutine/detail/flags.hpp"

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

template< typename Signature >
class coroutine_base : private noncopyable,
                       public coroutine_base_resume<
                            Signature,
                            coroutine_base< Signature >,
                            typename function_traits< Signature >::result_type,
                            function_traits< Signature >::arity
                       >
{
public:
    typedef intrusive_ptr< coroutine_base >     ptr_t;

private:
    template< typename X, typename Y, typename Z, int >
    friend class coroutine_base_resume;
    template< typename X, typename Y, typename Z, typename A, typename B, typename C, int >
    friend class coroutine_object;

    unsigned int            use_count_;
    context::fcontext_t     caller_;
    context::fcontext_t *   callee_;
    int                     flags_;
    exception_ptr           except_;

protected:
    virtual void deallocate_object() = 0;

public:
    coroutine_base( context::fcontext_t * callee, bool unwind, bool preserve_fpu) :
        coroutine_base_resume<
            Signature,
            coroutine_base< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        use_count_( 0),
        caller_(),
        callee_( callee),
        flags_( 0),
        except_()
    {
        if ( unwind) flags_ |= flag_force_unwind;
        if ( preserve_fpu) flags_ |= flag_preserve_fpu;
    }

    virtual ~coroutine_base()
    {}

    bool force_unwind() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_force_unwind); }

    bool unwind_requested() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_unwind_stack); }

    bool preserve_fpu() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_preserve_fpu); }

    bool is_complete() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_complete); }

    friend inline void intrusive_ptr_add_ref( coroutine_base * p) BOOST_NOEXCEPT
    { ++p->use_count_; }

    friend inline void intrusive_ptr_release( coroutine_base * p) BOOST_NOEXCEPT
    { if ( --p->use_count_ == 0) p->deallocate_object(); }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_COROUTINES_DETAIL_COROUTINE_BASE_H
