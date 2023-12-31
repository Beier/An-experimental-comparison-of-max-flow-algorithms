#ifndef BOOST_THREAD_ONCE_HPP
#define BOOST_THREAD_ONCE_HPP

//  once.hpp
//
//  (C) Copyright 2006-7 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "../thread/detail/platform.hpp"
#if defined(BOOST_THREAD_PLATFORM_WIN32)
#include "../thread/win32/once.hpp"
#elif defined(BOOST_THREAD_PLATFORM_PTHREAD)
#include "../thread/pthread/once.hpp"
#else
#error "Boost threads unavailable on this platform"
#endif

#include "../config/abi_prefix.hpp"

namespace boost
{
  // template<class Callable, class ...Args> void
  // call_once(once_flag& flag, Callable&& func, Args&&... args);
    inline void call_once(void (*func)(),once_flag& flag)
    {
        call_once(flag,func);
    }
}

#include "../config/abi_suffix.hpp"

#endif
