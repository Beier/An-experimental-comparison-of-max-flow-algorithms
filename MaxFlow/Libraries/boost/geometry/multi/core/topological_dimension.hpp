// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_GEOMETRY_MULTI_TOPOLOGICAL_DIMENSION_HPP
#define BOOST_GEOMETRY_MULTI_TOPOLOGICAL_DIMENSION_HPP


#include "../../../mpl/int.hpp"


#include "../../../geometry/core/topological_dimension.hpp"
#include "../../../geometry/multi/core/tags.hpp"


namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DISPATCH
namespace core_dispatch
{

template <>
struct top_dim<multi_point_tag> : boost::mpl::int_<0> {};


template <>
struct top_dim<multi_linestring_tag> : boost::mpl::int_<1> {};


template <>
struct top_dim<multi_polygon_tag> : boost::mpl::int_<2> {};


} // namespace core_dispatch
#endif


}} // namespace boost::geometry


#endif
