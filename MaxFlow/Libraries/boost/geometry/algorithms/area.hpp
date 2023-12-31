// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_AREA_HPP
#define BOOST_GEOMETRY_ALGORITHMS_AREA_HPP

#include "../../concept_check.hpp"
#include "../../mpl/if.hpp"
#include "../../range/functions.hpp"
#include "../../range/metafunctions.hpp"
#include "../../variant/static_visitor.hpp"
#include "../../variant/apply_visitor.hpp"

#include "../../geometry/core/closure.hpp"
#include "../../geometry/core/exterior_ring.hpp"
#include "../../geometry/core/interior_rings.hpp"
#include "../../geometry/core/point_order.hpp"
#include "../../geometry/core/ring_type.hpp"

#include "../../geometry/geometries/concepts/check.hpp"
#include "../../geometry/geometries/variant.hpp"

#include "../../geometry/algorithms/detail/calculate_null.hpp"
#include "../../geometry/algorithms/detail/calculate_sum.hpp"
// #include "../../geometry/algorithms/detail/throw_on_empty_input.hpp"

#include "../../geometry/strategies/area.hpp"
#include "../../geometry/strategies/default_area_result.hpp"

#include "../../geometry/strategies/concepts/area_concept.hpp"

#include "../../geometry/util/math.hpp"
#include "../../geometry/util/order_as_direction.hpp"
#include "../../geometry/views/closeable_view.hpp"
#include "../../geometry/views/reversible_view.hpp"


namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace area
{

struct box_area
{
    template <typename Box, typename Strategy>
    static inline typename coordinate_type<Box>::type
    apply(Box const& box, Strategy const&)
    {
        // Currently only works for 2D Cartesian boxes
        assert_dimension<Box, 2>();

        return (get<max_corner, 0>(box) - get<min_corner, 0>(box))
             * (get<max_corner, 1>(box) - get<min_corner, 1>(box));
    }
};


template
<
    iterate_direction Direction,
    closure_selector Closure
>
struct ring_area
{
    template <typename Ring, typename Strategy>
    static inline typename Strategy::return_type
    apply(Ring const& ring, Strategy const& strategy)
    {
        BOOST_CONCEPT_ASSERT( (geometry::concept::AreaStrategy<Strategy>) );
        assert_dimension<Ring, 2>();

        // Ignore warning (because using static method sometimes) on strategy
        boost::ignore_unused_variable_warning(strategy);

        // An open ring has at least three points,
        // A closed ring has at least four points,
        // if not, there is no (zero) area
        if (int(boost::size(ring))
                < core_detail::closure::minimum_ring_size<Closure>::value)
        {
            return typename Strategy::return_type();
        }

        typedef typename reversible_view<Ring const, Direction>::type rview_type;
        typedef typename closeable_view
            <
                rview_type const, Closure
            >::type view_type;
        typedef typename boost::range_iterator<view_type const>::type iterator_type;

        rview_type rview(ring);
        view_type view(rview);
        typename Strategy::state_type state;
        iterator_type it = boost::begin(view);
        iterator_type end = boost::end(view);

        for (iterator_type previous = it++;
            it != end;
            ++previous, ++it)
        {
            strategy.apply(*previous, *it, state);
        }

        return strategy.result(state);
    }
};


}} // namespace detail::area


#endif // DOXYGEN_NO_DETAIL


#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{

template
<
    typename Geometry,
    typename Tag = typename tag<Geometry>::type
>
struct area : detail::calculate_null
{
    template <typename Strategy>
    static inline typename Strategy::return_type apply(Geometry const& geometry, Strategy const& strategy)
    {
        return calculate_null::apply<typename Strategy::return_type>(geometry, strategy);
    }
};


template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
struct area<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>, void>
{
    template <typename Strategy>
    struct visitor: boost::static_visitor<typename Strategy::return_type>
    {
        Strategy const& m_strategy;

        visitor(Strategy const& strategy): m_strategy(strategy) {}

        template <typename Geometry>
        typename Strategy::return_type operator()(Geometry const& geometry) const
        {
            return dispatch::area<Geometry>::apply(geometry, m_strategy);
        }
    };

    template <typename Variant, typename Strategy>
    static inline typename Strategy::return_type
    apply(Variant const& variant_geometry, Strategy const& strategy)
    {
        return boost::apply_visitor(visitor<Strategy>(strategy), variant_geometry);
    }
};


template <typename Geometry>
struct area<Geometry, box_tag> : detail::area::box_area
{};


template <typename Ring>
struct area<Ring, ring_tag>
    : detail::area::ring_area
        <
            order_as_direction<geometry::point_order<Ring>::value>::value,
            geometry::closure<Ring>::value
        >
{};


template <typename Polygon>
struct area<Polygon, polygon_tag> : detail::calculate_polygon_sum
{
    template <typename Strategy>
    static inline typename Strategy::return_type apply(Polygon const& polygon, Strategy const& strategy)
    {
        return calculate_polygon_sum::apply<
            typename Strategy::return_type,
            detail::area::ring_area
                <
                    order_as_direction<geometry::point_order<Polygon>::value>::value,
                    geometry::closure<Polygon>::value
                >
            >(polygon, strategy);
    }
};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH



/*!
\brief \brief_calc{area}
\ingroup area
\details \details_calc{area}. \details_default_strategy

The area algorithm calculates the surface area of all geometries having a surface, namely
box, polygon, ring, multipolygon. The units are the square of the units used for the points
defining the surface. If subject geometry is defined in meters, then area is calculated
in square meters.

The area calculation can be done in all three common coordinate systems, Cartesian, Spherical
and Geographic as well.

\tparam Geometry \tparam_geometry
\param geometry \param_geometry
\return \return_calc{area}

\qbk{[include reference/algorithms/area.qbk]}
\qbk{[heading Examples]}
\qbk{[area] [area_output]}
*/
template <typename Geometry>
inline typename default_area_result<Geometry>::type area(Geometry const& geometry)
{
    concept::check<Geometry const>();

    typedef typename point_type<Geometry>::type point_type;
    typedef typename strategy::area::services::default_strategy
        <
            typename cs_tag<point_type>::type,
            point_type
        >::type strategy_type;

    // detail::throw_on_empty_input(geometry);
        
    return dispatch::area<Geometry>::apply(geometry, strategy_type());
}

/*!
\brief \brief_calc{area} \brief_strategy
\ingroup area
\details \details_calc{area} \brief_strategy. \details_strategy_reasons
\tparam Geometry \tparam_geometry
\tparam Strategy \tparam_strategy{Area}
\param geometry \param_geometry
\param strategy \param_strategy{area}
\return \return_calc{area}

\qbk{distinguish,with strategy}

\qbk{
[include reference/algorithms/area.qbk]

[heading Example]
[area_with_strategy]
[area_with_strategy_output]

[heading Available Strategies]
\* [link geometry.reference.strategies.strategy_area_surveyor Surveyor (cartesian)]
\* [link geometry.reference.strategies.strategy_area_huiller Huiller (spherical)]
}
 */
template <typename Geometry, typename Strategy>
inline typename Strategy::return_type area(
        Geometry const& geometry, Strategy const& strategy)
{
    concept::check<Geometry const>();

    // detail::throw_on_empty_input(geometry);
    
    return dispatch::area<Geometry>::apply(geometry, strategy);
}


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_ALGORITHMS_AREA_HPP
