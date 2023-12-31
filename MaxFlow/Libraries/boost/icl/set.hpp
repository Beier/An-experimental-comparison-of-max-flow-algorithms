/*-----------------------------------------------------------------------------+
Copyright (c) 2007-2011: Joachim Faulhaber
+------------------------------------------------------------------------------+
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENCE.txt or copy at
           http://www.boost.org/LICENSE_1_0.txt)
+-----------------------------------------------------------------------------*/
#ifndef BOOST_ICL_SET_HPP_JOFA_070519
#define BOOST_ICL_SET_HPP_JOFA_070519

#include "../icl/impl_config.hpp"

#if defined(ICL_USE_BOOST_MOVE_IMPLEMENTATION)
#   include "../container/set.hpp"
#elif defined(ICL_USE_STD_IMPLEMENTATION)
#   include <set>
#else 
#   include <set>
#endif

#include "../icl/associative_element_container.hpp"
#include "../icl/functors.hpp"

#endif // BOOST_ICL_SET_HPP_JOFA_070519

