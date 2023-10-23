// Boost.Units - A C++ library for zero-overhead dimensional analysis and 
// unit/quantity manipulation and conversion
//
// Copyright (C) 2003-2008 Matthias Christian Schabel
// Copyright (C) 2008 Steven Watanabe
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNITS_CGS_PRESSURE_HPP
#define BOOST_UNITS_CGS_PRESSURE_HPP

#include "../../../units/systems/cgs/base.hpp"
#include "../../../units/physical_dimensions/pressure.hpp"

namespace boost {

namespace units { 

namespace cgs {

typedef unit<pressure_dimension,cgs::system>     pressure;
    
BOOST_UNITS_STATIC_CONSTANT(barye,pressure);    
BOOST_UNITS_STATIC_CONSTANT(baryes,pressure);   

} // namespace cgs

} // namespace units

} // namespace boost

#endif // BOOST_UNITS_CGS_PRESSURE_HPP