/*
   Copyright (c) Marshall Clow 2011-2012.

   Distributed under the Boost Software License, Version 1.0. (See accompanying
   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

/// \file  is_permutation.hpp
/// \brief Is a sequence a permutation of another sequence
/// \author Marshall Clow

#ifndef BOOST_ALGORITHM_IS_PERMUTATION_HPP
#define BOOST_ALGORITHM_IS_PERMUTATION_HPP

#include <algorithm>    // for std::less, tie, mismatch and is_permutation (if available)
#include <utility>      // for std::make_pair
#include <functional>   // for std::equal_to
#include <iterator>

#include "../../range/begin.hpp"
#include "../../range/end.hpp"
#include "../../utility/enable_if.hpp"
#include "../../type_traits/is_same.hpp"
#include "../../tr1/tr1/tuple"      // for tie

namespace boost { namespace algorithm {

#if __cplusplus >= 201103L
//  Use the C++11 versions of is_permutation if it is available
using std::is_permutation;              // Section 25.2.12
#else
/// \cond DOXYGEN_HIDE
namespace detail {
    template <typename Predicate, typename Iterator>
    struct value_predicate {
        value_predicate ( Predicate p, Iterator it ) : p_ ( p ), it_ ( it ) {}

        template <typename T1>
        bool operator () ( const T1 &t1 ) const { return p_ ( *it_, t1 ); }
    private:
        Predicate &p_;
        Iterator it_;
        };
}
/// \endcond


/// \fn is_permutation ( ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 first2, BinaryPredicate p )
/// \brief Tests to see if the sequence [first,last) is a permutation of the sequence starting at first2
///
/// \param first    The start of the input sequence
/// \param last     One past the end of the input sequence
/// \param first2   The start of the second sequence
/// \param p        The predicate to compare elements with
///
/// \note           This function is part of the C++2011 standard library.
///  We will use the standard one if it is available,
///     otherwise we have our own implementation.
template< class ForwardIterator1, class ForwardIterator2, class BinaryPredicate >
bool is_permutation ( ForwardIterator1 first1, ForwardIterator1 last1,
                      ForwardIterator2 first2, BinaryPredicate p )
{
//  Skip the common prefix (if any)
//  std::tie (first1, first2) = std::mismatch (first1, last1, first2, p);
    std::pair<ForwardIterator1, ForwardIterator2> eq = std::mismatch (first1, last1, first2, p);
    first1 = eq.first;
    first2 = eq.second;
    if ( first1 != last1 ) {
    //  Create last2
        ForwardIterator2 last2 = first2;
        std::advance ( last2, std::distance (first1, last1));

    //  for each unique value in the sequence [first1,last1), count how many times
    //  it occurs, and make sure it occurs the same number of times in [first2, last2)
        for ( ForwardIterator1 iter = first1; iter != last1; ++iter ) {
            detail::value_predicate<BinaryPredicate, ForwardIterator1> pred ( p, iter );

        /*  For each value we haven't seen yet... */
            if ( std::find_if ( first1, iter, pred ) == iter ) {
                std::size_t dest_count = std::count_if ( first2, last2, pred );
                if ( dest_count == 0 || dest_count != (std::size_t) std::count_if ( iter, last1, pred ))
                    return false;
                }
            }
        }

    return true;
}

/// \fn is_permutation ( ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 first2 )
/// \brief Tests to see if the sequence [first,last) is a permutation of the sequence starting at first2
///
/// \param first    The start of the input sequence
/// \param last     One past the end of the input sequence
/// \param first2   The start of the second sequence
/// \note           This function is part of the C++2011 standard library.
///  We will use the standard one if it is available,
///     otherwise we have our own implementation.
template< class ForwardIterator1, class ForwardIterator2 >
bool is_permutation ( ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 first2 )
{
//  How should I deal with the idea that ForwardIterator1::value_type
//  and ForwardIterator2::value_type could be different? Define my own comparison predicate?
    return boost::algorithm::is_permutation ( first, last, first2,
                          std::equal_to<typename std::iterator_traits<ForwardIterator1>::value_type> ());
}

#endif

/// \fn is_permutation ( const Range &r, ForwardIterator first2 )
/// \brief Tests to see if the sequence [first,last) is a permutation of the sequence starting at first2
///
/// \param r        The input range
/// \param first2   The start of the second sequence
template <typename Range, typename ForwardIterator>
bool is_permutation ( const Range &r, ForwardIterator first2 )
{
    return boost::algorithm::is_permutation (boost::begin (r), boost::end (r), first2 );
}

/// \fn is_permutation ( const Range &r, ForwardIterator first2, BinaryPredicate pred )
/// \brief Tests to see if the sequence [first,last) is a permutation of the sequence starting at first2
///
/// \param r        The input range
/// \param first2   The start of the second sequence
/// \param pred     The predicate to compare elements with
///
//  Disable this template when the first two parameters are the same type
//  That way the non-range version will be chosen.
template <typename Range, typename ForwardIterator, typename BinaryPredicate>
typename boost::disable_if_c<boost::is_same<Range, ForwardIterator>::value, bool>::type
is_permutation ( const Range &r, ForwardIterator first2, BinaryPredicate pred )
{
    return boost::algorithm::is_permutation (boost::begin (r), boost::end (r), first2, pred );
}

}}

#endif  // BOOST_ALGORITHM_IS_PERMUTATION_HPP
