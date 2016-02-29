#ifndef SIMPLE_REACTOR_REACTOR_S01_POLLER_H_
#define SIMPLE_REACTOR_REACTOR_S01_POLLER_H_

#include <map>
#include <vector>

#include <time/timestamp.h>

struct pollfd;

namespace simple_reactor {

class Channel;
class EventLoop;

class Poller {
 public:

  explicit Poller(EventLoop* loop);
  ~Poller();

  Timestamp Poll(int timeout_micseconds, std::vector<Channel*>* active_channels);

  void UpdateChannel(Channel* channel);

 private:

  void FillActiveChannel(int numEvents, std::vector<Channel*>* channels) const;

  EventLoop*                 ownerloop_;
  std::vector<struct pollfd> pollfds_;
  std::map<int, Channel*>    channels_;
}; // class Poller

} // namespace simple_reactor

#endif  // SIMPLE_REACTOR_REACTOR_S01_POLLER_H_
