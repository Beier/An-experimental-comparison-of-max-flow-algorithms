// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_CORE_POINT_TYPE_HPP
#define BOOST_GEOMETRY_CORE_POINT_TYPE_HPP


#include "../../mpl/assert.hpp"
#include "../../range.hpp"
#include "../../type_traits/remove_const.hpp"

#include "../../geometry/core/ring_type.hpp"
#include "../../geometry/core/tag.hpp"
#include "../../geometry/core/tags.hpp"
#include "../../geometry/util/bare_type.hpp"

namespace boost { namespace geometry
{

namespace traits
{

/*!
\brief Traits class indicating the type of contained points
\ingroup traits
\par Geometries:
    - all geometries except point
\par Specializations should provide:
    - typedef P type (where P should fulfil the Point concept)
\tparam Geometry geometry
*/
template <typename Geometry>
struct point_type
{
    BOOST_MPL_ASSERT_MSG
        (
            false, NOT_IMPLEMENTED_FOR_THIS_POINT_TYPE, (types<Geometry>)
        );
};


} // namespace traits


#ifndef DOXYGEN_NO_DISPATCH
namespace core_dispatch
{

template <typename Tag, typename Geometry>
struct point_type
{
    // Default: call traits to get point type
    typedef typename boost::remove_const
        <
            typename traits::point_type<Geometry>::type
        >::type type;
};


// Specialization for point: the point itself
template <typename Point>
struct point_type<point_tag, Point>
{
    typedef Point type;
};


// Specializations for linestring/ring, via boost::range
template <typename Linestring>
struct point_type<linestring_tag, Linestring>
{
    typedef typename boost::range_value<Linestring>::type type;
};


template <typename Ring>
struct point_type<ring_tag, Ring>
{
    typedef typename boost::range_value<Ring>::type type;
};


// Specialization for polygon: the point-type is the point-type of its rings
template <typename Polygon>
struct point_type<polygon_tag, Polygon>
{
    typedef typename point_type
        <
            ring_tag,
            typename ring_type<polygon_tag, Polygon>::type
        >::type type;
};


} // namespace core_dispatch
#endif // DOXYGEN_NO_DISPATCH


/*!
\brief \brief_meta{type, point_type, \meta_geometry_type}
\tparam Geometry \tparam_geometry 
\ingroup core

\qbk{[include reference/core/point_type.qbk]}
*/
template <typename Geometry>
struct point_type
{
    typedef typename core_dispatch::point_type
        <
            typename tag<Geometry>::type,
            typename boost::geometry::util::bare_type<Geometry>::type
        >::type type;
};


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_CORE_POINT_TYPE_HPP
