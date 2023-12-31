
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_DETAIL_COROUTINE_BASE_RESUME_H
#define BOOST_COROUTINES_DETAIL_COROUTINE_BASE_RESUME_H

#include <iterator>

#include "../../assert.hpp"
#include "../../config.hpp"
#include "../../context/fcontext.hpp"
#include "../../optional.hpp"
#include "../../preprocessor/arithmetic/add.hpp"
#include "../../preprocessor/arithmetic/sub.hpp"
#include "../../preprocessor/cat.hpp"
#include "../../preprocessor/punctuation/comma_if.hpp"
#include "../../preprocessor/repetition/repeat_from_to.hpp"
#include "../../range.hpp"

#include "../../coroutine/detail/arg.hpp"
#include "../../coroutine/detail/config.hpp"
#include "../../coroutine/detail/exceptions.hpp"
#include "../../coroutine/detail/holder.hpp"

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

template< typename Signature, typename D, typename Result, int arity >
class coroutine_base_resume;

template< typename Signature, typename D >
class coroutine_base_resume< Signature, D, void, 0 >
{
public:
    void resume()
    {
        BOOST_ASSERT( static_cast< D * >( this)->callee_);

        holder< void > hldr_to( & static_cast< D * >( this)->caller_);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >( context::jump_fcontext(
            hldr_to.ctx,
            static_cast< D * >( this)->callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            static_cast< D * >( this)->preserve_fpu() ) ) );
        static_cast< D * >( this)->callee_ = hldr_from->ctx;
        if ( hldr_from->force_unwind) throw forced_unwind();
        if ( static_cast< D * >( this)->except_)
            rethrow_exception( static_cast< D * >( this)->except_);
    }
};

template< typename Signature, typename D, typename Result >
class coroutine_base_resume< Signature, D, Result, 0 >
{
public:
    void resume()
    {
        BOOST_ASSERT( static_cast< D * >( this));
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() );
        BOOST_ASSERT( static_cast< D * >( this)->callee_);

        holder< void > hldr_to( & static_cast< D * >( this)->caller_);
        holder< Result > * hldr_from(
            reinterpret_cast< holder< Result > * >( context::jump_fcontext(
            hldr_to.ctx,
            static_cast< D * >( this)->callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            static_cast< D * >( this)->preserve_fpu() ) ) );
        static_cast< D * >( this)->callee_ = hldr_from->ctx;
        result_ = hldr_from->data;
        if ( hldr_from->force_unwind) throw forced_unwind();
        if ( static_cast< D * >( this)->except_)
            rethrow_exception( static_cast< D * >( this)->except_);
    }

protected:
    template< typename X, typename Y, int >
    friend struct coroutine_get;

    optional< Result >  result_;
};

template< typename Signature, typename D >
class coroutine_base_resume< Signature, D, void, 1 >
{
public:
    typedef typename arg< Signature >::type     arg_type;

    void resume( arg_type a1)
    {
        BOOST_ASSERT( static_cast< D * >( this));
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() );
        BOOST_ASSERT( static_cast< D * >( this)->callee_);

        holder< arg_type > hldr_to( & static_cast< D * >( this)->caller_, a1);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >( context::jump_fcontext(
            hldr_to.ctx,
            static_cast< D * >( this)->callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            static_cast< D * >( this)->preserve_fpu() ) ) );
        static_cast< D * >( this)->callee_ = hldr_from->ctx;
        if ( hldr_from->force_unwind) throw forced_unwind();
        if ( static_cast< D * >( this)->except_)
            rethrow_exception( static_cast< D * >( this)->except_);
    }
};

template< typename Signature, typename D, typename Result >
class coroutine_base_resume< Signature, D, Result, 1 >
{
public:
    typedef typename arg< Signature >::type     arg_type;

    void resume( arg_type a1)
    {
        BOOST_ASSERT( static_cast< D * >( this));
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() );
        BOOST_ASSERT( static_cast< D * >( this)->callee_);

        context::fcontext_t caller;
        holder< arg_type > hldr_to( & static_cast< D * >( this)->caller_, a1);
        holder< Result > * hldr_from(
            reinterpret_cast< holder< Result > * >( context::jump_fcontext(
            hldr_to.ctx,
            static_cast< D * >( this)->callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            static_cast< D * >( this)->preserve_fpu() ) ) );
        static_cast< D * >( this)->callee_ = hldr_from->ctx;
        result_ = hldr_from->data;
        if ( hldr_from->force_unwind) throw forced_unwind();
        if ( static_cast< D * >( this)->except_)
            rethrow_exception( static_cast< D * >( this)->except_);
    }

protected:
    template< typename X, typename Y, int >
    friend struct coroutine_get;

    optional< Result >  result_;
};

#define BOOST_COROUTINE_BASE_RESUME_COMMA(n) BOOST_PP_COMMA_IF(BOOST_PP_SUB(n,1))
#define BOOST_COROUTINE_BASE_RESUME_VAL(z,n,unused) BOOST_COROUTINE_BASE_RESUME_COMMA(n) BOOST_PP_CAT(a,n)
#define BOOST_COROUTINE_BASE_RESUME_VALS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_COROUTINE_BASE_RESUME_VAL,~)
#define BOOST_COROUTINE_BASE_RESUME_ARG_TYPE(n) \
    typename function_traits< Signature >::BOOST_PP_CAT(BOOST_PP_CAT(arg,n),_type)
#define BOOST_COROUTINE_BASE_RESUME_ARG(z,n,unused) BOOST_COROUTINE_BASE_RESUME_COMMA(n) BOOST_COROUTINE_BASE_RESUME_ARG_TYPE(n) BOOST_PP_CAT(a,n)
#define BOOST_COROUTINE_BASE_RESUME_ARGS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_COROUTINE_BASE_RESUME_ARG,~)
#define BOOST_COROUTINE_BASE_RESUME(z,n,unused) \
template< typename Signature, typename D > \
class coroutine_base_resume< Signature, D, void, n > \
{ \
public: \
    typedef typename arg< Signature >::type     arg_type; \
\
    void resume( BOOST_COROUTINE_BASE_RESUME_ARGS(n)) \
    { \
        BOOST_ASSERT( static_cast< D * >( this)); \
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() ); \
        BOOST_ASSERT( static_cast< D * >( this)->callee_); \
\
        holder< arg_type > hldr_to( \
            & static_cast< D * >( this)->caller_, \
            arg_type(BOOST_COROUTINE_BASE_RESUME_VALS(n) ) ); \
        holder< void > * hldr_from( \
            reinterpret_cast< holder< void > * >( context::jump_fcontext( \
                hldr_to.ctx, \
                static_cast< D * >( this)->callee_, \
                reinterpret_cast< intptr_t >( & hldr_to), \
                static_cast< D * >( this)->preserve_fpu() ) ) ); \
        static_cast< D * >( this)->callee_ = hldr_from->ctx; \
        if ( hldr_from->force_unwind) throw forced_unwind(); \
        if ( static_cast< D * >( this)->except_) \
            rethrow_exception( static_cast< D * >( this)->except_); \
    } \
}; \
\
template< typename Signature, typename D, typename Result > \
class coroutine_base_resume< Signature, D, Result, n > \
{ \
public: \
    typedef typename arg< Signature >::type     arg_type; \
\
    void resume( BOOST_COROUTINE_BASE_RESUME_ARGS(n)) \
    { \
        BOOST_ASSERT( static_cast< D * >( this)); \
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() ); \
        BOOST_ASSERT( static_cast< D * >( this)->callee_); \
\
        holder< arg_type > hldr_to( \
            & static_cast< D * >( this)->caller_, \
            arg_type(BOOST_COROUTINE_BASE_RESUME_VALS(n) ) ); \
        holder< Result > * hldr_from( \
            reinterpret_cast< holder< Result > * >( context::jump_fcontext( \
                hldr_to.ctx, \
                static_cast< D * >( this)->callee_, \
                reinterpret_cast< intptr_t >( & hldr_to), \
                static_cast< D * >( this)->preserve_fpu() ) ) ); \
        static_cast< D * >( this)->callee_ = hldr_from->ctx; \
        result_ = hldr_from->data; \
        if ( hldr_from->force_unwind) throw forced_unwind(); \
        if ( static_cast< D * >( this)->except_) \
            rethrow_exception( static_cast< D * >( this)->except_); \
    } \
\
protected: \
    template< typename X, typename Y, int > \
    friend struct coroutine_get; \
\
    optional< Result >  result_; \
};
BOOST_PP_REPEAT_FROM_TO(2,11,BOOST_COROUTINE_BASE_RESUME,~)
#undef BOOST_COROUTINE_BASE_RESUME
#undef BOOST_COROUTINE_BASE_RESUME_ARGS
#undef BOOST_COROUTINE_BASE_RESUME_ARG
#undef BOOST_COROUTINE_BASE_RESUME_ARG_TYPE
#undef BOOST_COROUTINE_BASE_RESUME_VALS
#undef BOOST_COROUTINE_BASE_RESUME_VAL
#undef BOOST_COROUTINE_BASE_RESUME_COMMA

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_COROUTINES_DETAIL_coroutine_base_resume_H
