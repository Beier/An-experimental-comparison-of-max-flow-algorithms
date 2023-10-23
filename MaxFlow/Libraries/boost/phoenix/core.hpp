/*=============================================================================
 Copyright (c) 2001-2010 Joel de Guzman
 
 Distributed under the Boost Software License, Version 1.0. (See accompanying 
 file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ==============================================================================*/
#ifndef BOOST_PHOENIX_CORE_HPP
#define BOOST_PHOENIX_CORE_HPP

#ifdef BOOST_PHOENIX_USE_V2_OVER_V3
#include "../spirit/home/phoenix/core.hpp"
#else
#include "../phoenix/version.hpp"
#include "../phoenix/core/limits.hpp"
#include "../phoenix/core/actor.hpp"
#include "../phoenix/core/is_actor.hpp"
#include "../phoenix/core/argument.hpp"
#include "../phoenix/core/value.hpp"
#include "../phoenix/core/reference.hpp"
#include "../phoenix/core/nothing.hpp"
#include "../phoenix/core/function_equal.hpp"
#include "../phoenix/core/visit_each.hpp"
#include "../phoenix/core/v2_eval.hpp"
#endif

#endif
