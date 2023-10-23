// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_GEOMETRY_HPP
#define BOOST_GEOMETRY_GEOMETRY_HPP

// Shortcut to include all header files

#include "../geometry/core/cs.hpp"
#include "../geometry/core/tag.hpp"
#include "../geometry/core/tag_cast.hpp"
#include "../geometry/core/tags.hpp"

// Core algorithms
#include "../geometry/core/access.hpp"
#include "../geometry/core/exterior_ring.hpp"
#include "../geometry/core/interior_rings.hpp"
#include "../geometry/core/radian_access.hpp"
#include "../geometry/core/topological_dimension.hpp"


#include "../geometry/arithmetic/arithmetic.hpp"
#include "../geometry/arithmetic/dot_product.hpp"

#include "../geometry/strategies/strategies.hpp"

#include "../geometry/algorithms/append.hpp"
#include "../geometry/algorithms/area.hpp"
#include "../geometry/algorithms/assign.hpp"
#include "../geometry/algorithms/buffer.hpp"
#include "../geometry/algorithms/centroid.hpp"
#include "../geometry/algorithms/clear.hpp"
#include "../geometry/algorithms/comparable_distance.hpp"
#include "../geometry/algorithms/convert.hpp"
#include "../geometry/algorithms/convex_hull.hpp"
#include "../geometry/algorithms/correct.hpp"
#include "../geometry/algorithms/covered_by.hpp"
#include "../geometry/algorithms/difference.hpp"
#include "../geometry/algorithms/disjoint.hpp"
#include "../geometry/algorithms/distance.hpp"
#include "../geometry/algorithms/envelope.hpp"
#include "../geometry/algorithms/equals.hpp"
#include "../geometry/algorithms/expand.hpp"
#include "../geometry/algorithms/for_each.hpp"
#include "../geometry/algorithms/intersection.hpp"
#include "../geometry/algorithms/intersects.hpp"
#include "../geometry/algorithms/length.hpp"
#include "../geometry/algorithms/make.hpp"
#include "../geometry/algorithms/num_geometries.hpp"
#include "../geometry/algorithms/num_interior_rings.hpp"
#include "../geometry/algorithms/num_points.hpp"
#include "../geometry/algorithms/overlaps.hpp"
#include "../geometry/algorithms/perimeter.hpp"
#include "../geometry/algorithms/reverse.hpp"
#include "../geometry/algorithms/simplify.hpp"
#include "../geometry/algorithms/sym_difference.hpp"
#include "../geometry/algorithms/touches.hpp"
#include "../geometry/algorithms/transform.hpp"
#include "../geometry/algorithms/union.hpp"
#include "../geometry/algorithms/unique.hpp"
#include "../geometry/algorithms/within.hpp"

// Include multi a.o. because it can give weird effects
// if you don't (e.g. area=0 of a multipolygon)
#include "../geometry/multi/multi.hpp"

// check includes all concepts
#include "../geometry/geometries/concepts/check.hpp"

#include "../geometry/util/for_each_coordinate.hpp"
#include "../geometry/util/math.hpp"
#include "../geometry/util/select_most_precise.hpp"
#include "../geometry/util/select_coordinate_type.hpp"
#include "../geometry/io/dsv/write.hpp"

#include "../geometry/views/box_view.hpp"
#include "../geometry/views/segment_view.hpp"

#include "../geometry/io/io.hpp"

#endif // BOOST_GEOMETRY_GEOMETRY_HPP
