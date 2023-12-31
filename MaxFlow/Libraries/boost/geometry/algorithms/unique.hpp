// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_UNIQUE_HPP
#define BOOST_GEOMETRY_ALGORITHMS_UNIQUE_HPP

#include <algorithm>

#include "../../range.hpp"
#include "../../typeof/typeof.hpp"

#include "../../geometry/core/interior_rings.hpp"
#include "../../geometry/core/mutable_range.hpp"
#include "../../geometry/geometries/concepts/check.hpp"
#include "../../geometry/policies/compare.hpp"


namespace boost { namespace geometry
{


#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace unique
{


struct range_unique
{
    template <typename Range, typename ComparePolicy>
    static inline void apply(Range& range, ComparePolicy const& policy)
    {
        typename boost::range_iterator<Range>::type it
            = std::unique
                (
                    boost::begin(range),
                    boost::end(range),
                    policy
                );

        traits::resize<Range>::apply(range, it - boost::begin(range));
    }
};


struct polygon_unique
{
    template <typename Polygon, typename ComparePolicy>
    static inline void apply(Polygon& polygon, ComparePolicy const& policy)
    {
        typedef typename geometry::ring_type<Polygon>::type ring_type;

        range_unique::apply(exterior_ring(polygon), policy);

        typename interior_return_type<Polygon>::type rings
                    = interior_rings(polygon);
        for (BOOST_AUTO_TPL(it, boost::begin(rings)); it != boost::end(rings); ++it)
        {
            range_unique::apply(*it, policy);
        }
    }
};



}} // namespace detail::unique
#endif // DOXYGEN_NO_DETAIL



#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{


template
<
    typename Geometry,
    typename Tag = typename tag<Geometry>::type
>
struct unique
{
    template <typename ComparePolicy>
    static inline void apply(Geometry&, ComparePolicy const& )
    {}
};


template <typename Ring>
struct unique<Ring, ring_tag>
    : detail::unique::range_unique
{};


template <typename LineString>
struct unique<LineString, linestring_tag>
    : detail::unique::range_unique
{};


template <typename Polygon>
struct unique<Polygon, polygon_tag>
    : detail::unique::polygon_unique
{};


} // namespace dispatch
#endif


/*!
\brief \brief_calc{minimal set}
\ingroup unique
\details \details_calc{unique,minimal set (where duplicate consecutive points are removed)}.
\tparam Geometry \tparam_geometry
\param geometry \param_geometry which will be made unique

\qbk{[include reference/algorithms/unique.qbk]}
*/
template <typename Geometry>
inline void unique(Geometry& geometry)
{
    concept::check<Geometry>();

    // Default strategy is the default point-comparison policy
    typedef geometry::equal_to
        <
            typename geometry::point_type<Geometry>::type
        > policy;


    dispatch::unique<Geometry>::apply(geometry, policy());
}

}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_ALGORITHMS_UNIQUE_HPP
