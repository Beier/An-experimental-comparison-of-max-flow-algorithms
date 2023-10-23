//
// detail/impl/posix_tss_ptr.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_IMPL_POSIX_TSS_PTR_IPP
#define BOOST_ASIO_DETAIL_IMPL_POSIX_TSS_PTR_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "../../../asio/detail/config.hpp"

#if defined(BOOST_HAS_PTHREADS) && !defined(BOOST_ASIO_DISABLE_THREADS)

#include "../../../asio/detail/posix_tss_ptr.hpp"
#include "../../../asio/detail/throw_error.hpp"
#include "../../../asio/error.hpp"

#include "../../../asio/detail/push_options.hpp"

namespace boost {
namespace asio {
namespace detail {

void posix_tss_ptr_create(pthread_key_t& key)
{
  int error = ::pthread_key_create(&key, 0);
  boost::system::error_code ec(error,
      boost::asio::error::get_system_category());
  boost::asio::detail::throw_error(ec, "tss");
}

} // namespace detail
} // namespace asio
} // namespace boost

#include "../../../asio/detail/pop_options.hpp"

#endif // defined(BOOST_HAS_PTHREADS) && !defined(BOOST_ASIO_DISABLE_THREADS)

#endif // BOOST_ASIO_DETAIL_IMPL_POSIX_TSS_PTR_IPP
