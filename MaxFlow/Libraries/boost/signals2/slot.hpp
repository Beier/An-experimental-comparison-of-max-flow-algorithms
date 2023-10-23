// Boost.Signals2 library

// Copyright Frank Mori Hess 2009.
//
// Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS2_SLOT_HPP
#define BOOST_SIGNALS2_SLOT_HPP

#include "../bind.hpp"
#include "../config.hpp"
#include "../function.hpp"
#include "../mpl/identity.hpp"
#include "../ref.hpp"
#include "../signals2/detail/signals_common.hpp"
#include "../signals2/detail/signals_common_macros.hpp"
#include "../signals2/detail/tracked_objects_visitor.hpp"
#include "../signals2/slot_base.hpp"
#include "../visit_each.hpp"
#include "../weak_ptr.hpp"

#ifdef BOOST_NO_CXX11_VARIADIC_TEMPLATES
#include "../signals2/preprocessed_slot.hpp"
#else
#include "../signals2/variadic_slot.hpp"
#endif

#endif // BOOST_SIGNALS2_SLOT_HPP
