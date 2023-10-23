// Copyright (C) 2006 Douglas Gregor <doug.gregor -at- gmail.com>.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Message Passing Interface

//  See www.boost.org/libs/mpi for documentation.

/** @file mpi.hpp
 *
 *  This file is a top-level convenience header that includes all of
 *  the Boost.MPI library headers. Users concerned about compile time
 *  may wish to include only specific headers from the Boost.MPI
 *  library.
 *
 */
#ifndef BOOST_MPI_HPP
#define BOOST_MPI_HPP

#include "mpi/allocator.hpp"
#include "mpi/collectives.hpp"
#include "mpi/communicator.hpp"
#include "mpi/datatype.hpp"
#include "mpi/environment.hpp"
#include "mpi/graph_communicator.hpp"
#include "mpi/group.hpp"
#include "mpi/intercommunicator.hpp"
#include "mpi/nonblocking.hpp"
#include "mpi/operations.hpp"
#include "mpi/skeleton_and_content.hpp"
#include "mpi/timer.hpp"

#endif // BOOST_MPI_HPP
