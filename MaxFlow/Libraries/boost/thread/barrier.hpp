// Copyright (C) 2002-2003
// David Moore, William E. Kempf
// Copyright (C) 2007-8 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_BARRIER_JDM030602_HPP
#define BOOST_BARRIER_JDM030602_HPP

#include "../thread/detail/config.hpp"
#include "../throw_exception.hpp"

#include "../thread/mutex.hpp"
#include "../thread/lock_types.hpp"
#include "../thread/condition_variable.hpp"
#include <string>
#include <stdexcept>

#include "../config/abi_prefix.hpp"

namespace boost
{

    class barrier
    {
    public:
        barrier(unsigned int count)
            : m_threshold(count), m_count(count), m_generation(0)
        {
            if (count == 0)
                boost::throw_exception(thread_exception(system::errc::invalid_argument, "barrier constructor: count cannot be zero."));
        }

        bool wait()
        {
            boost::unique_lock<boost::mutex> lock(m_mutex);
            unsigned int gen = m_generation;

            if (--m_count == 0)
            {
                m_generation++;
                m_count = m_threshold;
                m_cond.notify_all();
                return true;
            }

            while (gen == m_generation)
                m_cond.wait(lock);
            return false;
        }

    private:
        mutex m_mutex;
        condition_variable m_cond;
        unsigned int m_threshold;
        unsigned int m_count;
        unsigned int m_generation;
    };

}   // namespace boost

#include "../config/abi_suffix.hpp"

#endif
