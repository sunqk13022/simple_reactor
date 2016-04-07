#ifndef SIMPLE_REACTOR_REACTOR_S12_EPOLLER_H_
#define SIMPLE_REACTOR_REACTOR_S12_EPOLLER_H_

#include <map>
#include <vector>

#include <time/timestamp.h>

#include "event_loop.h"

struct epoll_event;

namespace simple_reactor {

class Channel;

class EPoller {
 public:
  typedef std::vector<Channel*> ChannelList;

  explicit EPoller(EventLoop* loop);
  ~EPoller();

  Timestamp Poll(int timeout_micseconds, ChannelList* active_channels);

  void UpdateChannel(Channel* channel);
  void RemoveChannel(Channel* channel);

 private:
  static const int kInitEventListSize = 16;

  void FillActiveChannel(int numEvents, std::vector<Channel*>* channels) const;
  void Update(int operation, Channel* channel);

  typedef std::vector<struct epoll_event> EventList;
  typedef std::map<int, Channel*> ChannelMap;

  EventLoop*                 ownerloop_;
  int                        epollfd_;
  EventList                  events_;
  ChannelMap                 channels_;
}; // class EPoller

} // namespace simple_reactor

#endif  // SIMPLE_REACTOR_REACTOR_S12_EPOLLER_H_
