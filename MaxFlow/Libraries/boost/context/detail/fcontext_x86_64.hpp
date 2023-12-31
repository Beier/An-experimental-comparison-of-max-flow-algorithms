
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CONTEXT_DETAIL_FCONTEXT_X86_64_H
#define BOOST_CONTEXT_DETAIL_FCONTEXT_X86_64_H

#include <cstddef>

#include "../../config.hpp"
#include "../../cstdint.hpp"

#include "../../context/detail/config.hpp"

#ifdef BOOST_HAS_ABI_HEADERS
# include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace context {

extern "C" {

#define BOOST_CONTEXT_CALLDECL

struct stack_t
{
    void    *   sp;
    std::size_t size;

    stack_t() :
        sp( 0), size( 0)
    {}
};

struct fp_t
{
    boost::uint32_t     fc_freg[2];

    fp_t() :
        fc_freg()
    {}
};

struct fcontext_t
{
    boost::uint64_t     fc_greg[8];
    stack_t             fc_stack;
    fp_t                fc_fp;

    fcontext_t() :
        fc_greg(),
        fc_stack(),
        fc_fp()
    {}
};

}

}}

#ifdef BOOST_HAS_ABI_HEADERS
# include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CONTEXT_DETAIL_FCONTEXT_X86_64_H
