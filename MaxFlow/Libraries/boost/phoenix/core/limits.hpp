/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_PHOENIX_CORE_LIMITS_HPP
#define BOOST_PHOENIX_CORE_LIMITS_HPP

#include "../../preprocessor/arithmetic/add.hpp"
#include "../../preprocessor/inc.hpp"
#include "../../preprocessor/dec.hpp"
#include "../../preprocessor/stringize.hpp"
#include "../../phoenix/version.hpp"
#include "../../phoenix/support/preprocessor/round.hpp"


#if defined(BOOST_PHOENIX_LIMIT)
#define BOOST_PROTO_MAX_ARITY BOOST_PHOENIX_LIMIT
#include "../../proto/proto_fwd.hpp"
#else
#include "../../proto/proto_fwd.hpp"
#define BOOST_PHOENIX_LIMIT BOOST_PROTO_MAX_ARITY
#endif

#define PHOENIX_LIMIT BOOST_PHOENIX_LIMIT

#define BOOST_PHOENIX_LIMIT_STR BOOST_PP_STRINGIZE(BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT))

#if !defined(BOOST_PHOENIX_ARG_LIMIT)
# define BOOST_PHOENIX_ARG_LIMIT BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT)
#elif (BOOST_PHOENIX_ARG_LIMIT < 5)
# error "BOOST_PHOENIX_ARG_LIMIT is set too low"
#elif BOOST_PHOENIX_ARG_LIMIT != BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT) && !defined(BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES)
# define BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES
#endif

#if !defined(BOOST_PHOENIX_ACTOR_LIMIT)
# define BOOST_PHOENIX_ACTOR_LIMIT BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT)
#elif (BOOST_PHOENIX_ACTOR_LIMIT > BOOST_PHOENIX_ARG_LIMIT)
# error "BOOST_PHOENIX_ACTOR_LIMIT > BOOST_PHOENIX_ARG_LIMIT"
#elif (BOOST_PHOENIX_ACTOR_LIMIT < 3)
# error "BOOST_PHOENIX_ACTOR_LIMIT is set too low"
#elif BOOST_PHOENIX_ACTOR_LIMIT != BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT) && !defined(BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES)
# define BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES
#endif

#if !defined(BOOST_PHOENIX_PERFECT_FORWARD_LIMIT)
# define BOOST_PHOENIX_PERFECT_FORWARD_LIMIT 3
#elif (BOOST_PHOENIX_PERFECT_FORWARD_LIMIT > BOOST_PHOENIX_ACTOR_LIMIT)
# error "BOOST_PHOENIX_PERFECT_FORWARD_LIMIT > BOOST_PHOENIX_ACTOR_LIMIT"
#elif (BOOST_PHOENIX_PERFECT_FORWARD_LIMIT < 3)
# error "BOOST_PHOENIX_PERFECT_FORWARD_LIMIT is set too low"
#elif BOOST_PHOENIX_PERFECT_FORWARD_LIMIT != 3 && !defined(BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES)
# define BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES
#endif

#if !defined(BOOST_PHOENIX_COMPOSITE_LIMIT)
# define BOOST_PHOENIX_COMPOSITE_LIMIT BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT)
#elif (BOOST_PHOENIX_COMPOSITE_LIMIT < 5)
# error "BOOST_PHOENIX_COMPOSITE_LIMIT is set too low"
#elif BOOST_PHOENIX_COMPOSITE_LIMIT != BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT) && !defined(BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES)
# define BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES
#endif

#if !defined(BOOST_PHOENIX_MEMBER_LIMIT)
# define BOOST_PHOENIX_MEMBER_LIMIT BOOST_PP_DEC(BOOST_PHOENIX_COMPOSITE_LIMIT)
#elif (BOOST_PHOENIX_MEMBER_LIMIT > BOOST_PHOENIX_COMPOSITE_LIMIT)
# error "BOOST_PHOENIX_MEMBER_LIMIT > BOOST_PHOENIX_COMPOSITE_LIMIT"
#elif (BOOST_PHOENIX_MEMBER_LIMIT < 3)
# error "BOOST_PHOENIX_MEMBER_LIMIT is set too low"
#elif BOOST_PHOENIX_MEMBER_LIMIT != BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT) && !defined(BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES)
# define BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES
#endif

#if !defined(BOOST_PHOENIX_CATCH_LIMIT)
# define BOOST_PHOENIX_CATCH_LIMIT BOOST_PHOENIX_COMPOSITE_LIMIT
#elif (BOOST_PHOENIX_CATCH_LIMIT < 1)
# error "BOOST_PHOENIX_CATCH_LIMIT is set too low"
#elif BOOST_PHOENIX_CATCH_LIMIT != BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT) && !defined(BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES)
# define BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES
#endif

#if !defined(BOOST_PHOENIX_DYNAMIC_LIMIT)
# define BOOST_PHOENIX_DYNAMIC_LIMIT BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT)
#elif (BOOST_PHOENIX_DYNAMIC_LIMIT < 1)
# error "BOOST_PHOENIX_DYNAMIC_LIMIT is set too low"
#elif BOOST_PHOENIX_DYNAMIC_LIMIT != BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT) && !defined(BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES)
# define BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES
#endif

#if !defined(BOOST_PHOENIX_LOCAL_LIMIT)
# define BOOST_PHOENIX_LOCAL_LIMIT BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT)
#elif (BOOST_PHOENIX_LOCAL_LIMIT < 3)
# error "BOOST_PHOENIX_LOCAL_LIMIT is set too low"
#elif BOOST_PHOENIX_LOCAL_LIMIT != BOOST_PHOENIX_PP_ROUND_UP(BOOST_PHOENIX_LIMIT) && !defined(BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES)
# define BOOST_PHOENIX_DONT_USE_PREPROCESSED_FILES
#endif

#endif
