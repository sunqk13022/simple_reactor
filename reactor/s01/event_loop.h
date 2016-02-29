#ifndef SIMPLE_REACTOR_S00_EVENT_LOOP_H_
#define SIMPLE_REACTOR_S00_EVENT_LOOP_H_

#include <thread/thread.h>

#include <boost/scoped_ptr.hpp>
#include <vector>

namespace simple_reactor {

class Channel;
class Poller;

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  void Loop();
  void Quit();

  void AssertInloopthread();

  bool IsInLoopthread() const;

  void UpdateChannel(Channel* ch);

 private:
  void AbortNotInloopthread();

  bool                       loop_;
  bool                       quit_;
  const pid_t                threadid_;
  boost::scoped_ptr<Poller>  poller_;
  std::vector<Channel*>      active_channel_;
}; // class EventLoop

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_S00_EVENT_LOOP_H_
