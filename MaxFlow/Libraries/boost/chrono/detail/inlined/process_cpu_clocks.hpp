//  boost process_cpu_clocks.cpp  -----------------------------------------------------------//

//  Copyright 2009-2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See http://www.boost.org/libs/chrono for documentation.

//--------------------------------------------------------------------------------------//
#ifndef BOOST_CHRONO_DETAIL_INLINED_PROCESS_CPU_CLOCKS_HPP
#define BOOST_CHRONO_DETAIL_INLINED_PROCESS_CPU_CLOCKS_HPP


#include "../../../chrono/config.hpp"
#if defined(BOOST_CHRONO_HAS_PROCESS_CLOCKS)

#include "../../../version.hpp"
#include "../../../chrono/process_cpu_clocks.hpp"
#include "../../../throw_exception.hpp"
#include "../../../system/system_error.hpp"

//----------------------------------------------------------------------------//
//                                Windows                                     //
//----------------------------------------------------------------------------//
#if defined(BOOST_CHRONO_WINDOWS_API)
#include "../../../chrono/detail/inlined/win/process_cpu_clocks.hpp"

//----------------------------------------------------------------------------//
//                                 Mac                                        //
//----------------------------------------------------------------------------//
#elif defined(BOOST_CHRONO_MAC_API)
#include "../../../chrono/detail/inlined/mac/process_cpu_clocks.hpp"

//----------------------------------------------------------------------------//
//                                POSIX                                     //
//----------------------------------------------------------------------------//
#elif defined(BOOST_CHRONO_POSIX_API)
#include "../../../chrono/detail/inlined/posix/process_cpu_clocks.hpp"

#endif  // POSIX

#endif

#endif
