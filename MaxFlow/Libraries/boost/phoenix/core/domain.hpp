/*=============================================================================
    Copyright (c) 2005-2010 Joel de Guzman
    Copyright (c) 2010 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_PHOENIX_CORE_DOMAIN_HPP
#define BOOST_PHOENIX_CORE_DOMAIN_HPP

#include "../../phoenix/core/limits.hpp"
#include "../../proto/matches.hpp"
#include "../../proto/transform/call.hpp"
#include "../../proto/transform/when.hpp"
#include "../../proto/domain.hpp"

namespace boost { namespace phoenix
{
    template <typename Expr>
    struct actor;
    
    struct meta_grammar;

    struct phoenix_generator
        : proto::switch_<phoenix_generator>
    {
        template<typename Tag>
        struct case_
            : proto::otherwise<proto::call<proto::pod_generator<actor>(proto::_)> >
        {};
    };

    struct phoenix_domain
        : proto::domain<
            phoenix_generator
          , meta_grammar
          , proto::basic_default_domain
        >
    {
        template <typename T>
        struct as_child
            : as_expr<T>
        {};
    };
}}

#endif
