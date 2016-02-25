#include "log_stream.h"

#include <limits>
#include <stdint.h>
#include <iostream>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace simple_reactor;

BOOST_AUTO_TEST_CASE(LogStreamBool) {
  std::cout << "bool test\n";
  LogStream os;
  const LogStream::Buffer& buf = os.GetBuffer();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string(""));
}
