/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#include "caf/io/network/event_handler.hpp"

#include "caf/logger.hpp"

#include "caf/io/network/default_multiplexer.hpp"

#ifdef CAF_WINDOWS
#  include <winsock2.h>
#else
#  include <sys/socket.h>
#endif

namespace caf::io::network {

event_handler::event_handler(default_multiplexer& dm, native_socket sockfd)
  : fd_(sockfd),
    state_{true, false, false, false,
           to_integer(receive_policy_flag::at_least)},
    eventbf_(0),
    backend_(dm) {
  set_fd_flags();
}

event_handler::~event_handler() {
  if (fd_ != invalid_native_socket) {
    CAF_LOG_DEBUG("close socket" << CAF_ARG(fd_));
    close_socket(fd_);
  }
}

void event_handler::passivate() {
  backend().del(operation::read, fd(), this);
}

void event_handler::activate() {
  backend().add(operation::read, fd(), this);
}

void event_handler::set_fd_flags() {
  if (fd_ == invalid_native_socket)
    return;
  // enable nonblocking IO, disable Nagle's algorithm, and suppress SIGPIPE
  nonblocking(fd_, true);
  tcp_nodelay(fd_, true);
  allow_sigpipe(fd_, false);
}

} // namespace caf
