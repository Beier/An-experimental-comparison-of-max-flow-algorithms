///////////////////////////////////////////////////////////////////////////////
/// \file algorithm.hpp
///   Includes the range-based versions of the algorithms in the
///   C++ standard header file <algorithm>
//
/////////////////////////////////////////////////////////////////////////////

// Copyright 2009 Neil Groves.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Acknowledgements:
// This code uses combinations of ideas, techniques and code snippets
// from: Thorsten Ottosen, Eric Niebler, Jeremy Siek,
// and Vladimir Prus'
//
// The original mutating algorithms that served as the first version
// were originally written by Vladimir Prus'
// <ghost@cs.msu.su> code from Boost Wiki

#if defined(_MSC_VER) && _MSC_VER >= 1000
#pragma once
#endif

#ifndef BOOST_RANGE_ALGORITHM_HPP_INCLUDED_01012009
#define BOOST_RANGE_ALGORITHM_HPP_INCLUDED_01012009

#include "../range/concepts.hpp"
#include "../range/iterator_range.hpp"
#include "../range/difference_type.hpp"
#include "../range/detail/range_return.hpp"
#include "../iterator/iterator_traits.hpp"
#include "../next_prior.hpp"
#include <algorithm>

// Non-mutating algorithms
#include "../range/algorithm/adjacent_find.hpp"
#include "../range/algorithm/count.hpp"
#include "../range/algorithm/count_if.hpp"
#include "../range/algorithm/equal.hpp"
#include "../range/algorithm/for_each.hpp"
#include "../range/algorithm/find.hpp"
#include "../range/algorithm/find_end.hpp"
#include "../range/algorithm/find_first_of.hpp"
#include "../range/algorithm/find_if.hpp"
#include "../range/algorithm/lexicographical_compare.hpp"
#include "../range/algorithm/mismatch.hpp"
#include "../range/algorithm/search.hpp"
#include "../range/algorithm/search_n.hpp"

// Mutating algorithms
#include "../range/algorithm/copy.hpp"
#include "../range/algorithm/copy_backward.hpp"
#include "../range/algorithm/fill.hpp"
#include "../range/algorithm/fill_n.hpp"
#include "../range/algorithm/generate.hpp"
#include "../range/algorithm/inplace_merge.hpp"
#include "../range/algorithm/merge.hpp"
#include "../range/algorithm/nth_element.hpp"
#include "../range/algorithm/partial_sort.hpp"
#include "../range/algorithm/partial_sort_copy.hpp"
#include "../range/algorithm/partition.hpp"
#include "../range/algorithm/random_shuffle.hpp"
#include "../range/algorithm/remove.hpp"
#include "../range/algorithm/remove_copy.hpp"
#include "../range/algorithm/remove_copy_if.hpp"
#include "../range/algorithm/remove_if.hpp"
#include "../range/algorithm/replace.hpp"
#include "../range/algorithm/replace_copy.hpp"
#include "../range/algorithm/replace_copy_if.hpp"
#include "../range/algorithm/replace_if.hpp"
#include "../range/algorithm/reverse.hpp"
#include "../range/algorithm/reverse_copy.hpp"
#include "../range/algorithm/rotate.hpp"
#include "../range/algorithm/rotate_copy.hpp"
#include "../range/algorithm/sort.hpp"
#include "../range/algorithm/stable_partition.hpp"
#include "../range/algorithm/stable_sort.hpp"
#include "../range/algorithm/transform.hpp"
#include "../range/algorithm/unique.hpp"
#include "../range/algorithm/unique_copy.hpp"

// Binary search
#include "../range/algorithm/binary_search.hpp"
#include "../range/algorithm/equal_range.hpp"
#include "../range/algorithm/lower_bound.hpp"
#include "../range/algorithm/upper_bound.hpp"

// Set operations of sorted ranges
#include "../range/algorithm/set_algorithm.hpp"

// Heap operations
#include "../range/algorithm/heap_algorithm.hpp"

// Minimum and Maximum
#include "../range/algorithm/max_element.hpp"
#include "../range/algorithm/min_element.hpp"

// Permutations
#include "../range/algorithm/permutation.hpp"

#endif // include guard

