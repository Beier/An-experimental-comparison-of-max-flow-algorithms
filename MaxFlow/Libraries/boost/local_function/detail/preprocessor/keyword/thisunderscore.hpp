
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.boost.org/LICENSE_1_0.txt)
// Home at http://www.boost.org/libs/local_function

#ifndef BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_HPP_
#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_HPP_

#include "../../../../local_function/detail/preprocessor/keyword/facility/is.hpp"
#include "../../../../local_function/detail/preprocessor/keyword/facility/add.hpp"
#include "../../../../local_function/detail/preprocessor/keyword/facility/remove.hpp"

// PRIVATE //

// These are not local macros -- DO NOT #UNDEF.
#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_ISthis_ (1)
#define this_BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_IS (1)
#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_REMOVEthis_
#define this_BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_REMOVE

// PUBLIC //

// Is.

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_THISUNDERSCORE_FRONT(tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_IS_FRONT(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_IS)

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_THISUNDERSCORE_BACK(token) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_IS_BACK(token, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_IS)

// Remove.

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_REMOVE_FRONT( \
        tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_REMOVE_FRONT(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_THISUNDERSCORE_FRONT, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_REMOVE)

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_REMOVE_BACK( \
        tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_REMOVE_BACK(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_THISUNDERSCORE_BACK, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_REMOVE)

// Add.

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_ADD_FRONT( \
        tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_ADD_FRONT(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_THISUNDERSCORE_FRONT, \
            this_)

#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_THISUNDERSCORE_ADD_BACK(tokens) \
    BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_ADD_BACK(tokens, \
            BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_IS_THISUNDERSCORE_BACK, \
            this_)

#endif // #include guard

