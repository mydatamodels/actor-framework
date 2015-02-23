/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright (C) 2011 - 2015                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#include <thread>
#include <atomic>

#include "test.hpp"

#include "caf/all.hpp"
#include "caf/io/all.hpp"

using namespace caf;

namespace {

std::atomic<long> s_dtor_called;

class dummy : public event_based_actor {
 public:
  ~dummy() {
    ++s_dtor_called;
  }
  behavior make_behavior() override {
    return {
      others() >> CAF_UNEXPECTED_MSG_CB(this)
    };
  }
};

uint16_t publish_at_some_port(uint16_t first_port, actor whom) {
  auto port = first_port;
  for (;;) {
    try {
      io::publish(whom, port);
      return port;
    }
    catch (bind_failure&) {
      // try next port
      ++port;
    }
  }
}

actor invalid_unpublish(uint16_t port) {
  auto d = spawn<dummy>();
  io::unpublish(d, port);
  anon_send_exit(d, exit_reason::user_shutdown);
  d = invalid_actor;
  return io::remote_actor("127.0.0.1", port);
}

} // namespace <anonymous>

int main() {
  CAF_TEST(test_unpublish);
  auto d = spawn<dummy>();
  auto port = publish_at_some_port(4242, d);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  CAF_CHECK_EQUAL(invalid_unpublish(port), d);
  io::unpublish(d, port);
  CAF_CHECKPOINT();
  // must fail now
  try {
    auto oops = io::remote_actor("127.0.0.1", port);
    CAF_FAILURE("unexpected: remote actor succeeded!");
  } catch (network_error&) {
    CAF_CHECKPOINT();
  }
  anon_send_exit(d, exit_reason::user_shutdown);
  d = invalid_actor;
  await_all_actors_done();
  shutdown();
  CAF_CHECK_EQUAL(s_dtor_called.load(), 2);
  return CAF_TEST_RESULT();
}
