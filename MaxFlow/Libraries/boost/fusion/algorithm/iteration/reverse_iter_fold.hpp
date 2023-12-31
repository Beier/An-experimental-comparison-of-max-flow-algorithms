/*=============================================================================
    Copyright (c) 2010 Christopher Schmidt
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_FUSION_ALGORITHM_ITERATION_REVERSE_ITER_FOLD_HPP
#define BOOST_FUSION_ALGORITHM_ITERATION_REVERSE_ITER_FOLD_HPP

#include "../../../fusion/algorithm/iteration/reverse_iter_fold_fwd.hpp"
#include "../../../config.hpp"
#include "../../../fusion/sequence/intrinsic/begin.hpp"
#include "../../../fusion/sequence/intrinsic/end.hpp"
#include "../../../fusion/sequence/intrinsic/empty.hpp"
#include "../../../fusion/sequence/intrinsic/size.hpp"
#include "../../../fusion/support/is_segmented.hpp"
#include "../../../fusion/iterator/equal_to.hpp"
#include "../../../fusion/iterator/deref.hpp"
#include "../../../fusion/iterator/value_of.hpp"
#include "../../../fusion/iterator/prior.hpp"
#include "../../../fusion/iterator/next.hpp"
#include "../../../mpl/eval_if.hpp"
#include "../../../mpl/if.hpp"
#include "../../../mpl/bool.hpp"
#include "../../../utility/result_of.hpp"
#include "../../../type_traits/add_const.hpp"
#include "../../../type_traits/add_reference.hpp"

#define BOOST_FUSION_REVERSE_FOLD
#define BOOST_FUSION_ITER_FOLD

#if !defined(BOOST_FUSION_DONT_USE_PREPROCESSED_FILES)
#include "../../../fusion/algorithm/iteration/detail/preprocessed/reverse_iter_fold.hpp"
#else
#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 2, line: 0, output: "detail/preprocessed/reverse_iter_fold.hpp")
#endif

/*=============================================================================
    Copyright (c) 2010 Christopher Schmidt
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    This is an auto-generated file. Do not edit!
==============================================================================*/

#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 1)
#endif

#include "../../../fusion/algorithm/iteration/detail/fold.hpp"

#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(output: null)
#endif

#endif // BOOST_FUSION_DONT_USE_PREPROCESSED_FILES

#undef BOOST_FUSION_REVERSE_FOLD
#undef BOOST_FUSION_ITER_FOLD

#endif
