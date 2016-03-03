#include "timestamp.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

namespace simple_reactor {

static const int kMicroSeconds = 1000 * 1000;

Timestamp::Timestamp() : ns_(0)
{
}

Timestamp::Timestamp(int64_t t) : ns_(t)
{
}

Timestamp Timestamp::Now() {
  struct timeval tv = {0, 0};
  gettimeofday(&tv, NULL);
  return Timestamp(tv.tv_sec * kMicroSeconds + tv.tv_usec);
}

std::string Timestamp::ToString() const {
  char buf[32] = {0};
  int64_t seconds = ns_ / kMicroSeconds;
  int64_t microseconds = ns_ % kMicroSeconds;
  snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
  return buf;
}

std::string Timestamp::ToFormattedString() const {
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(ns_ / kMicroSeconds);
  int microseconds = static_cast<int>(ns_ % kMicroSeconds);

  struct tm tm_time;
  localtime_r(&seconds, &tm_time);
  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
           tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
           tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
           microseconds);
  return buf;
}

time_t Timestamp::Seconds() const {
  return static_cast<time_t>(ns_ / kMicroSeconds);
}

void Timestamp::AddSeconds(double seconds) {
  ns_ += seconds * kMicroSeconds;
}

} // namespace simple_reactor
