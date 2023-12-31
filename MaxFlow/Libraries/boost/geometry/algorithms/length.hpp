// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_LENGTH_HPP
#define BOOST_GEOMETRY_ALGORITHMS_LENGTH_HPP

#include <iterator>

#include "../../range.hpp"

#include "../../mpl/if.hpp"
#include "../../type_traits.hpp"

#include "../../geometry/core/cs.hpp"
#include "../../geometry/core/closure.hpp"

#include "../../geometry/geometries/concepts/check.hpp"

#include "../../geometry/algorithms/assign.hpp"
#include "../../geometry/algorithms/detail/calculate_null.hpp"
// #include "../../geometry/algorithms/detail/throw_on_empty_input.hpp"
#include "../../geometry/views/closeable_view.hpp"
#include "../../geometry/strategies/distance.hpp"
#include "../../geometry/strategies/default_length_result.hpp"


namespace boost { namespace geometry
{


#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace length
{


template<typename Segment>
struct segment_length
{
    template <typename Strategy>
    static inline typename default_length_result<Segment>::type apply(
            Segment const& segment, Strategy const& strategy)
    {
        typedef typename point_type<Segment>::type point_type;
        point_type p1, p2;
        geometry::detail::assign_point_from_index<0>(segment, p1);
        geometry::detail::assign_point_from_index<1>(segment, p2);
        return strategy.apply(p1, p2);
    }
};

/*!
\brief Internal, calculates length of a linestring using iterator pairs and
    specified strategy
\note for_each could be used here, now that point_type is changed by boost
    range iterator
*/
template<typename Range, closure_selector Closure>
struct range_length
{
    typedef typename default_length_result<Range>::type return_type;

    template <typename Strategy>
    static inline return_type apply(
            Range const& range, Strategy const& strategy)
    {
        typedef typename closeable_view<Range const, Closure>::type view_type;
        typedef typename boost::range_iterator
            <
                view_type const
            >::type iterator_type;

        return_type sum = return_type();
        view_type view(range);
        iterator_type it = boost::begin(view), end = boost::end(view);
        if(it != end)
        {
            for(iterator_type previous = it++;
                    it != end;
                    ++previous, ++it)
            {
                // Add point-point distance using the return type belonging
                // to strategy
                sum += strategy.apply(*previous, *it);
            }
        }

        return sum;
    }
};


}} // namespace detail::length
#endif // DOXYGEN_NO_DETAIL


#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{


template <typename Geometry, typename Tag = typename tag<Geometry>::type>
struct length : detail::calculate_null
{
    typedef typename default_length_result<Geometry>::type return_type;

    template <typename Strategy>
    static inline return_type apply(Geometry const& geometry, Strategy const& strategy)
    {
        return calculate_null::apply<return_type>(geometry, strategy);
    }
};


template <typename Geometry>
struct length<Geometry, linestring_tag>
    : detail::length::range_length<Geometry, closed>
{};


// RING: length is currently 0; it might be argued that it is the "perimeter"


template <typename Geometry>
struct length<Geometry, segment_tag>
    : detail::length::segment_length<Geometry>
{};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


/*!
\brief \brief_calc{length}
\ingroup length
\details \details_calc{length, length (the sum of distances between consecutive points)}. \details_default_strategy
\tparam Geometry \tparam_geometry
\param geometry \param_geometry
\return \return_calc{length}

\qbk{[include reference/algorithms/length.qbk]}
\qbk{[length] [length_output]}
 */
template<typename Geometry>
inline typename default_length_result<Geometry>::type length(
        Geometry const& geometry)
{
    concept::check<Geometry const>();

    // detail::throw_on_empty_input(geometry);

    typedef typename strategy::distance::services::default_strategy
        <
            point_tag, typename point_type<Geometry>::type
        >::type strategy_type;

    return dispatch::length<Geometry>::apply(geometry, strategy_type());
}


/*!
\brief \brief_calc{length} \brief_strategy
\ingroup length
\details \details_calc{length, length (the sum of distances between consecutive points)} \brief_strategy. \details_strategy_reasons
\tparam Geometry \tparam_geometry
\tparam Strategy \tparam_strategy{distance}
\param geometry \param_geometry
\param strategy \param_strategy{distance}
\return \return_calc{length}

\qbk{distinguish,with strategy}
\qbk{[include reference/algorithms/length.qbk]}
\qbk{[length_with_strategy] [length_with_strategy_output]}
 */
template<typename Geometry, typename Strategy>
inline typename default_length_result<Geometry>::type length(
        Geometry const& geometry, Strategy const& strategy)
{
    concept::check<Geometry const>();

    // detail::throw_on_empty_input(geometry);
    
    return dispatch::length<Geometry>::apply(geometry, strategy);
}


}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ALGORITHMS_LENGTH_HPP
