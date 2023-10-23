/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_VECTOR30_05052005_0206)
#define FUSION_VECTOR30_05052005_0206

#include "../../../fusion/container/vector/vector30_fwd.hpp"
#include "../../../fusion/support/sequence_base.hpp"
#include "../../../fusion/support/detail/access.hpp"
#include "../../../fusion/iterator/next.hpp"
#include "../../../fusion/iterator/deref.hpp"
#include "../../../fusion/sequence/intrinsic/begin.hpp"
#include "../../../fusion/container/vector/detail/at_impl.hpp"
#include "../../../fusion/container/vector/detail/value_at_impl.hpp"
#include "../../../fusion/container/vector/detail/begin_impl.hpp"
#include "../../../fusion/container/vector/detail/end_impl.hpp"

#include "../../../mpl/void.hpp"
#include "../../../mpl/int.hpp"
#include "../../../mpl/at.hpp"
#include "../../../mpl/bool.hpp"
#include "../../../mpl/vector/vector30.hpp"
#include "../../../type_traits/is_convertible.hpp"
#include "../../../utility/enable_if.hpp"

#include "../../../preprocessor/dec.hpp"
#include "../../../preprocessor/iteration/iterate.hpp"
#include "../../../preprocessor/repetition/enum.hpp"
#include "../../../preprocessor/repetition/enum_shifted.hpp"
#include "../../../preprocessor/repetition/enum_params.hpp"
#include "../../../preprocessor/repetition/enum_binary_params.hpp"
#include "../../../preprocessor/repetition/repeat_from_to.hpp"

#if !defined(BOOST_FUSION_DONT_USE_PREPROCESSED_FILES)
#include "../../../fusion/container/vector/detail/preprocessed/vector30.hpp"
#else
#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 2, line: 0, output: "detail/preprocessed/vector30.hpp")
#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    This is an auto-generated file. Do not edit!
==============================================================================*/

#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 1)
#endif

namespace boost { namespace fusion
{
    struct vector_tag;
    struct fusion_sequence_tag;
    struct random_access_traversal_tag;

// expand vector21 to vector30
#define BOOST_PP_FILENAME_1 "../../../fusion/container/vector/detail/vector_n.hpp"
#define BOOST_PP_ITERATION_LIMITS (21, 30)
#include BOOST_PP_ITERATE()

}}

#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(output: null)
#endif

#endif // BOOST_FUSION_DONT_USE_PREPROCESSED_FILES

#endif

