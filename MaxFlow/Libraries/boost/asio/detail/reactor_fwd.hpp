//
// detail/reactor_fwd.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_REACTOR_FWD_HPP
#define BOOST_ASIO_DETAIL_REACTOR_FWD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "../../asio/detail/config.hpp"

#if defined(BOOST_ASIO_HAS_IOCP)
# include "../../asio/detail/select_reactor_fwd.hpp"
#elif defined(BOOST_ASIO_HAS_EPOLL)
# include "../../asio/detail/epoll_reactor_fwd.hpp"
#elif defined(BOOST_ASIO_HAS_KQUEUE)
# include "../../asio/detail/kqueue_reactor_fwd.hpp"
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
# include "../../asio/detail/dev_poll_reactor_fwd.hpp"
#else
# include "../../asio/detail/select_reactor_fwd.hpp"
#endif

namespace boost {
namespace asio {
namespace detail {

#if defined(BOOST_ASIO_HAS_IOCP)
typedef select_reactor reactor;
#elif defined(BOOST_ASIO_HAS_EPOLL)
typedef epoll_reactor reactor;
#elif defined(BOOST_ASIO_HAS_KQUEUE)
typedef kqueue_reactor reactor;
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
typedef dev_poll_reactor reactor;
#else
typedef select_reactor reactor;
#endif

} // namespace detail
} // namespace asio
} // namespace boost

#endif // BOOST_ASIO_DETAIL_REACTOR_FWD_HPP
