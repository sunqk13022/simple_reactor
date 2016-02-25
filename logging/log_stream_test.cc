#include "log_stream.h"

#include <limits>
#include <stdint.h>
#include <iostream>
#include <stdio.h>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace simple_reactor;

BOOST_AUTO_TEST_CASE(LogStreamBool) {
  //std::cout << "bool test\n";
  LogStream os;
  const LogStream::Buffer& buf = os.GetBuffer();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string(""));

  os << true;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("1"));
  os << '\n';
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("1\n"));
  os << false;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("1\n0"));
}

BOOST_AUTO_TEST_CASE(LogStreamInt) {
  LogStream os;
  const LogStream::Buffer& buf = os.GetBuffer();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string(""));

  os << 1;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("1"));
  os << '0';
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("10"));
  os << -1;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("10-1"));
  os.ResetBuffer();

  os << 0 << " " << 123 << 'x' << 0x64;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0 123x100"));
}

BOOST_AUTO_TEST_CASE(LogStreamIntLimit) {
  LogStream os;
  const LogStream::Buffer& buf = os.GetBuffer();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string(""));

  os << -2147483647;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("-2147483647"));
  os << (int)-2147483648;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("-2147483647-2147483648"));
  os << ' ';
  os << 2147483647;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("-2147483647-2147483648 2147483647"));

  os.ResetBuffer();
  os << std::numeric_limits<short>::min();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("-32768"));

  os.ResetBuffer();
  os << std::numeric_limits<short>::max();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("32767"));

  os.ResetBuffer();
  os << std::numeric_limits<unsigned short>::min();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0"));

  os.ResetBuffer();
  os << std::numeric_limits<unsigned short>::max();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("65535"));

  os.ResetBuffer();
  os << std::numeric_limits<int>::min();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("-2147483648"));

  os.ResetBuffer();
  os << std::numeric_limits<int>::max();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("2147483647"));

  os.ResetBuffer();
  os << std::numeric_limits<unsigned int>::min();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0"));

  os.ResetBuffer();
  os << std::numeric_limits<unsigned int>::max();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("4294967295"));

  os.ResetBuffer();
  os << std::numeric_limits<long>::min();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("-9223372036854775808"));

  os.ResetBuffer();
  os << std::numeric_limits<long>::max();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("9223372036854775807"));

  os.ResetBuffer();
  os << std::numeric_limits<unsigned long>::min();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0"));

  os.ResetBuffer();
  os << std::numeric_limits<unsigned long>::max();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("18446744073709551615"));

  short a = 0;
  int b = 0;
  long c = 0;
  os.ResetBuffer();
  os << a << b << c;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("000"));
}

BOOST_AUTO_TEST_CASE(LogStreamfloat) {
  //std::cout << "bool test\n";
  LogStream os;
  const LogStream::Buffer& buf = os.GetBuffer();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string(""));

  os.ResetBuffer();
  os << 0.0;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0"));
  //printf("float = %.12g\n", 0.0);

  os.ResetBuffer();
  os << 1.0;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("1"));

  os.ResetBuffer();
  os << 0.1;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0.1"));

  os.ResetBuffer();
  os << 0.05;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0.05"));

  os.ResetBuffer();
  os << 0.15;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0.15"));

  os.ResetBuffer();
  os << (double)0.15;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0.15"));

  os.ResetBuffer();
  os << 1.23456789;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("1.23456789"));

  os.ResetBuffer();
  os << -123.456;
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("-123.456"));
}

BOOST_AUTO_TEST_CASE(LogStreamVoid) {
  LogStream os;
  const LogStream::Buffer& buf = os.GetBuffer();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string(""));

  os.ResetBuffer();
  os << static_cast<void*>(0);
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0x0"));

  os.ResetBuffer();
  os << reinterpret_cast<void*>(8888);
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("0x22b8"));
}

BOOST_AUTO_TEST_CASE(LogStreamString) {
  LogStream os;
  const LogStream::Buffer& buf = os.GetBuffer();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string(""));

  os.ResetBuffer();
  os << std::string("simple_reactor");
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("simple_reactor"));
  os << std::string("simple_reactor");
  BOOST_CHECK_EQUAL(buf.ToString(), std::string("simple_reactorsimple_reactor"));
}

BOOST_AUTO_TEST_CASE(LogStreamLong) {
  LogStream os;
  const LogStream::Buffer& buf = os.GetBuffer();
  BOOST_CHECK_EQUAL(buf.ToString(), std::string(""));

  for (int i = 0; i < 399; ++i) {
    os << "123456789 ";
    BOOST_CHECK_EQUAL(buf.Length(), 10*(i+1));
    BOOST_CHECK_EQUAL(buf.Avail(), 4000 - 10*(i+1));
  }
  //os.ResetBuffer();
  //std::cout << buf.Length();
  os << "abcdefghi ";
  BOOST_CHECK_EQUAL(buf.Length(), 3990);
  BOOST_CHECK_EQUAL(buf.Avail(), 10);

  os << "abcdefghi";
  BOOST_CHECK_EQUAL(buf.Length(), 3999);
  BOOST_CHECK_EQUAL(buf.Avail(), 1);
}
