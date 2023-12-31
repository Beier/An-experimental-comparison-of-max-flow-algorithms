#ifndef BOOST_RECURSIVE_MUTEX_WIN32_HPP
#define BOOST_RECURSIVE_MUTEX_WIN32_HPP

//  recursive_mutex.hpp
//
//  (C) Copyright 2006-7 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include "../../thread/win32/basic_recursive_mutex.hpp"
#include "../../thread/exceptions.hpp"
#include "../../thread/detail/delete.hpp"
#if defined BOOST_THREAD_PROVIDES_NESTED_LOCKS
#include "../../thread/lock_types.hpp"
#endif

#include "../../config/abi_prefix.hpp"

namespace boost
{
    class recursive_mutex:
        public ::boost::detail::basic_recursive_mutex
    {
    public:
        BOOST_THREAD_NO_COPYABLE(recursive_mutex)
        recursive_mutex()
        {
            ::boost::detail::basic_recursive_mutex::initialize();
        }
        ~recursive_mutex()
        {
            ::boost::detail::basic_recursive_mutex::destroy();
        }

#if defined BOOST_THREAD_PROVIDES_NESTED_LOCKS
        typedef unique_lock<recursive_mutex> scoped_lock;
        typedef detail::try_lock_wrapper<recursive_mutex> scoped_try_lock;
#endif
    };

    typedef recursive_mutex recursive_try_mutex;

    class recursive_timed_mutex:
        public ::boost::detail::basic_recursive_timed_mutex
    {
    public:
        BOOST_THREAD_NO_COPYABLE(recursive_timed_mutex)
        recursive_timed_mutex()
        {
            ::boost::detail::basic_recursive_timed_mutex::initialize();
        }
        ~recursive_timed_mutex()
        {
            ::boost::detail::basic_recursive_timed_mutex::destroy();
        }

#if defined BOOST_THREAD_PROVIDES_NESTED_LOCKS
        typedef unique_lock<recursive_timed_mutex> scoped_timed_lock;
        typedef detail::try_lock_wrapper<recursive_timed_mutex> scoped_try_lock;
        typedef scoped_timed_lock scoped_lock;
#endif
    };
}

#include "../../config/abi_suffix.hpp"

#endif
