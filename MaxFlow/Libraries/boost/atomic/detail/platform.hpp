#ifndef BOOST_ATOMIC_DETAIL_PLATFORM_HPP
#define BOOST_ATOMIC_DETAIL_PLATFORM_HPP

//  Copyright (c) 2009 Helge Bahmann
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// Platform selection file

#include "../../atomic/detail/config.hpp"

#ifdef BOOST_ATOMIC_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

    #include "../../atomic/detail/gcc-x86.hpp"

#elif 0 && defined(__GNUC__) && defined(__alpha__) /* currently does not work correctly */

    #include "../../atomic/detail/base.hpp"
    #include "../../atomic/detail/gcc-alpha.hpp"

#elif defined(__GNUC__) && (defined(__POWERPC__) || defined(__PPC__))

    #include "../../atomic/detail/gcc-ppc.hpp"

// This list of ARM architecture versions comes from Apple's arm/arch.h header.
// I don't know how complete it is.
#elif defined(__GNUC__) && (defined(__ARM_ARCH_6__)  || defined(__ARM_ARCH_6J__) \
                         || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__) \
                         || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_7A__))

    #include "../../atomic/detail/gcc-armv6plus.hpp"

#elif defined(__linux__) && defined(__arm__)

    #include "../../atomic/detail/linux-arm.hpp"

#elif defined(__GNUC__) && defined(__sparc_v9__)

    #include "../../atomic/detail/gcc-sparcv9.hpp"

#elif defined(BOOST_WINDOWS) || defined(_WIN32_CE)

    #include "../../atomic/detail/windows.hpp"

#elif 0 && defined(__GNUC__) /* currently does not work correctly */

    #include "../../atomic/detail/base.hpp"
    #include "../../atomic/detail/gcc-cas.hpp"

#else

#include "../../atomic/detail/base.hpp"

#endif

#endif
