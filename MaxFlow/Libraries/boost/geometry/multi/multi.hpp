// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_MULTI_HPP
#define BOOST_GEOMETRY_MULTI_HPP


#include "../../geometry/multi/core/closure.hpp"
#include "../../geometry/multi/core/geometry_id.hpp"
#include "../../geometry/multi/core/interior_rings.hpp"
#include "../../geometry/multi/core/is_areal.hpp"
#include "../../geometry/multi/core/point_order.hpp"
#include "../../geometry/multi/core/point_type.hpp"
#include "../../geometry/multi/core/ring_type.hpp"
#include "../../geometry/multi/core/tags.hpp"
#include "../../geometry/multi/core/topological_dimension.hpp"

#include "../../geometry/multi/algorithms/append.hpp"
#include "../../geometry/multi/algorithms/area.hpp"
#include "../../geometry/multi/algorithms/centroid.hpp"
#include "../../geometry/multi/algorithms/clear.hpp"
#include "../../geometry/multi/algorithms/convert.hpp"
#include "../../geometry/multi/algorithms/correct.hpp"
#include "../../geometry/multi/algorithms/covered_by.hpp"
#include "../../geometry/multi/algorithms/disjoint.hpp"
#include "../../geometry/multi/algorithms/distance.hpp"
#include "../../geometry/multi/algorithms/envelope.hpp"
#include "../../geometry/multi/algorithms/equals.hpp"
#include "../../geometry/multi/algorithms/for_each.hpp"
#include "../../geometry/multi/algorithms/intersection.hpp"
#include "../../geometry/multi/algorithms/length.hpp"
#include "../../geometry/multi/algorithms/num_geometries.hpp"
#include "../../geometry/multi/algorithms/num_interior_rings.hpp"
#include "../../geometry/multi/algorithms/num_points.hpp"
#include "../../geometry/multi/algorithms/perimeter.hpp"
#include "../../geometry/multi/algorithms/reverse.hpp"
#include "../../geometry/multi/algorithms/simplify.hpp"
#include "../../geometry/multi/algorithms/transform.hpp"
#include "../../geometry/multi/algorithms/unique.hpp"
#include "../../geometry/multi/algorithms/within.hpp"

#include "../../geometry/multi/algorithms/detail/for_each_range.hpp"
#include "../../geometry/multi/algorithms/detail/modify_with_predicate.hpp"
#include "../../geometry/multi/algorithms/detail/multi_sum.hpp"

#include "../../geometry/multi/algorithms/detail/sections/range_by_section.hpp"
#include "../../geometry/multi/algorithms/detail/sections/sectionalize.hpp"

#include "../../geometry/multi/algorithms/detail/overlay/copy_segment_point.hpp"
#include "../../geometry/multi/algorithms/detail/overlay/copy_segments.hpp"
#include "../../geometry/multi/algorithms/detail/overlay/get_ring.hpp"
#include "../../geometry/multi/algorithms/detail/overlay/get_turns.hpp"
#include "../../geometry/multi/algorithms/detail/overlay/select_rings.hpp"
#include "../../geometry/multi/algorithms/detail/overlay/self_turn_points.hpp"

#include "../../geometry/multi/geometries/concepts/check.hpp"
#include "../../geometry/multi/geometries/concepts/multi_point_concept.hpp"
#include "../../geometry/multi/geometries/concepts/multi_linestring_concept.hpp"
#include "../../geometry/multi/geometries/concepts/multi_polygon_concept.hpp"

#include "../../geometry/multi/views/detail/range_type.hpp"
#include "../../geometry/multi/strategies/cartesian/centroid_average.hpp"

#include "../../geometry/multi/io/dsv/write.hpp"
#include "../../geometry/multi/io/wkt/wkt.hpp"


#endif // BOOST_GEOMETRY_MULTI_HPP
