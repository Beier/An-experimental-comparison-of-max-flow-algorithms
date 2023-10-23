//
// detail/winsock_init.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_WINSOCK_INIT_HPP
#define BOOST_ASIO_DETAIL_WINSOCK_INIT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "../../asio/detail/config.hpp"

#if defined(BOOST_WINDOWS) || defined(__CYGWIN__)

#include "../../asio/detail/push_options.hpp"

namespace boost {
namespace asio {
namespace detail {

class winsock_init_base
{
protected:
  // Structure to track result of initialisation and number of uses. POD is used
  // to ensure that the values are zero-initialised prior to any code being run.
  struct data
  {
    long init_count_;
    long result_;
  };

  BOOST_ASIO_DECL static void startup(data& d,
      unsigned char major, unsigned char minor);

  BOOST_ASIO_DECL static void cleanup(data& d);

  BOOST_ASIO_DECL static void throw_on_error(data& d);
};

template <int Major = 2, int Minor = 0>
class winsock_init : private winsock_init_base
{
public:
  winsock_init(bool allow_throw = true)
  {
    startup(data_, Major, Minor);
    if (allow_throw)
      throw_on_error(data_);
  }

  winsock_init(const winsock_init&)
  {
    startup(data_, Major, Minor);
    throw_on_error(data_);
  }

  ~winsock_init()
  {
    cleanup(data_);
  }

private:
  static data data_;
};

template <int Major, int Minor>
winsock_init_base::data winsock_init<Major, Minor>::data_;

// Static variable to ensure that winsock is initialised before main, and
// therefore before any other threads can get started.
static const winsock_init<>& winsock_init_instance = winsock_init<>(false);

} // namespace detail
} // namespace asio
} // namespace boost

#include "../../asio/detail/pop_options.hpp"

#if defined(BOOST_ASIO_HEADER_ONLY)
# include "../../asio/detail/impl/winsock_init.ipp"
#endif // defined(BOOST_ASIO_HEADER_ONLY)

#endif // defined(BOOST_WINDOWS) || defined(__CYGWIN__)

#endif // BOOST_ASIO_DETAIL_WINSOCK_INIT_HPP
