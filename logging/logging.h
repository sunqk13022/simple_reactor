#ifndef SIMPLE_REACTOR_LOGGING_LOGGING_H_
#define SIMPLE_REACTOR_LOGGING_LOGGING_H_

#include <boost/scoped_ptr.hpp>
#include "log_stream.h"
#include "time/timestamp.h"

namespace simple_reactor {

class Logger {
 public:
  enum LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

  Logger(const char* file, int line, LogLevel level, const char* fname);
  ~Logger();

  LogStream& Stream();

  static LogLevel GetLogLevel();
  static void SetLogLevel(LogLevel level);

  typedef void (*OutputFunctor)(const char* msg, int len);
  typedef void (*FlushFunctor)();
  static void SetOutput(OutputFunctor);
  static void SetFlush(FlushFunctor);

 private:

  struct Impl {
    Impl(LogLevel level, const char* file, int line);
    void FormatTime();
    void Finish();

    Timestamp     time_;
    LogStream     stream_;
    LogLevel      level_;
    int           line_;
    const char*   fullname_;
    const char*   basename_;
  }; // class Impl

  Impl impl_;
}; // class Logger

#define LOG_TRACE

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_LOGGING_LOGGING_H_
