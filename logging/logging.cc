#include "logging.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

#include <thread/thread.h>

namespace simple_reactor {

__thread char   t_errnobuf[512];
__thread char   t_time[32];
__thread time_t t_last_second;

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

Logger::LogLevel g_log_level = Logger::INFO;

static void defaultOutput(const char* msg, int len) {
  fwrite(msg, 1, len, stdout);
}

static void defaultFlush() {
  fflush(stdout);
}

Logger::OutputFunctor g_output = defaultOutput;
Logger::FlushFunctor  g_flush  = defaultFlush;

Logger::Impl::Impl(LogLevel level, const char* file, int line)
  : time_(Timestamp::Now()),
    stream_(),
    level_(level),
    line_(line),
    fullname_(file),
    basename_(NULL)
{
  const char* path_sep_pos = strrchr(fullname_, '/');
  basename_ = (path_sep_pos != NULL) ? path_sep_pos + 1 : fullname_;

  FormatTime();

  Fmt tid("%5d ", CurrentThread::Tid());
  stream_ << tid;
  stream_ << LogLevelName[level_];
}

void Logger::Impl::FormatTime() {
  int64_t us = time_.Ns();
  time_t seconds = time_.Seconds();
  int microseconds = static_cast<int>(us % 1000000);

  if (seconds != t_last_second) {
    t_last_second = seconds;

    struct tm tm_time;
    localtime_r(&seconds, &tm_time);
    snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900,
             tm_time.tm_mon + 1,
             tm_time.tm_mday,
             tm_time.tm_hour,
             tm_time.tm_min,
             tm_time.tm_sec);
  }

  stream_.Append(t_time, 17);
  Fmt uc(".%06dZ ", microseconds);
  stream_ << us;
}

void Logger::Impl::Finish() {
  stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(const char* file, int line, LogLevel level, const char* fname)
: impl_(level, file, line) {
  impl_.stream_ << fname << ' ';
}
Logger::~Logger() {
  impl_.Finish();

  const LogStream::Buffer& buf(impl_.stream_.GetBuffer());
  g_output(buf.Data(), buf.Length());

  if (impl_.level_ == FATAL) {
    g_flush();
    abort();
  }
}

Logger::LogLevel Logger::GetLogLevel() {
  return g_log_level;
}

void Logger::SetLogLevel(LogLevel level) {
  g_log_level = level;
}
void Logger::SetOutput(OutputFunctor f) {
  g_output = f;
}

void Logger::SetFlush(FlushFunctor f) {
  g_flush = f;
}

} // namespace simple_reactor
