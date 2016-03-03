#ifndef SIMPLE_REACTOR_REACTOR_SO2_CALLBACK_H_
#define SIMPLE_REACTOR_REACTOR_SO2_CALLBACK_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "time/timestamp.h"

namespace simple_reactor {

typedef boost::function<void ()> TimerCallback;

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_SO2_CALLBACK_H_
