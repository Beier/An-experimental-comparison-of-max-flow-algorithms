
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.boost.org/LICENSE_1_0.txt)
// Home at http://www.boost.org/libs/local_function

#ifndef BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_HPP_
#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_HPP_

#include "../../../../local_function/detail/preprocessor/keyword/facility/is.hpp"
#include "../../../../local_function/detail/preprocessor/keyword/facility/add.hpp"
#include "../../../../local_function/detail/preprocessor/keyword/facility/remove.hpp"

// PRIVATE //

// These are not local macros -- DO NOT #UNDEF.
#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_IS_inline (1) /* unary */
#define inline_BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_IS (1) /* unary */
#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_REMOVE_inline/* nothing */
#define inline_BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_REMOVE/* nothing */

// PUBLIC //

// Is.

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_INLINE_FRONT(tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_IS_FRONT(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_IS_)

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_INLINE_BACK(token) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_IS_BACK(token, \
            _BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_IS)

// Remove.

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_REMOVE_FRONT(tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_REMOVE_FRONT(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_INLINE_FRONT, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_REMOVE_)

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_REMOVE_BACK(tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_REMOVE_BACK(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_INLINE_BACK, \
            _BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_REMOVE)

// Add.

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_ADD_FRONT(tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_ADD_FRONT(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_INLINE_FRONT, inline)

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_INLINE_ADD_BACK(tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_ADD_BACK(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_INLINE_BACK, inline)

#endif // #include guard

