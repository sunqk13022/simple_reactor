#include "epoller.h"

#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>

#include <logging/logging.h>
#include "channel.h"

namespace simple_reactor {

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPoller::EPoller(EventLoop* loop)
  : ownerloop_(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{
  if (epollfd_ < 0) {
    LOG_FATAL << "Epoll:: create error!";
  }
}

EPoller::~EPoller() {
  ::close(epollfd_);
}

void EPoller::FillActiveChannel(int numEvents, ChannelList* channels) const {
  for (int i = 0; i < numEvents; ++i) {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    channel->SetReventsFd(events_[i].events);
    channels->push_back(channel);
  }
}

void EPoller::UpdateChannel(Channel* channel) {
  ownerloop_->AssertInloopthread();
  //LOG_INFO << "fd=" << channel->Fd() << " events=" << channel->Events();
  const int index = channel->Index();
  if (index == kNew || index == kDeleted) {
    int fd = channel->Fd();
    if (index == kNew) {
      channels_[fd] = channel;
    }
    channel->SetIndex(kAdded);
    Update(EPOLL_CTL_ADD, channel);
  } else {
    //int fd = channel->Fd();
    if (channel->IsNoneEvent()) {
      Update(EPOLL_CTL_DEL, channel);
      channel->SetIndex(kDeleted);
    } else {
      Update(EPOLL_CTL_MOD, channel);
    }
  }
}

Timestamp EPoller::Poll(int timeout_micseconds, std::vector<Channel*>* active_channels) {
  int numEvents = ::epoll_wait(epollfd_,
                               &*events_.begin(),
                               (int)(events_.size()),
                               timeout_micseconds);
  Timestamp now = Timestamp::Now();

  if (numEvents > 0) {
    //LOG_INFO << numEvents << " events happened";
    FillActiveChannel(numEvents, active_channels);
    if ((size_t)numEvents == events_.size()) {
      events_.resize(events_.size() * 2);
    }
  } else if (numEvents == 0) {
    //LOG_INFO << "nothing happened";
  } else {
    LOG_FATAL << "EPOLL::Poll()";
  }

  return now;
}

void EPoller::RemoveChannel(Channel* channel) {
  ownerloop_->AssertInloopthread();
  //LOG_INFO << "fd=" << channel->Fd();
  int fd = channel->Fd();
  int index = channel->Index();
  channels_.erase(fd);
  if (index == kAdded) {
    Update(EPOLL_CTL_DEL, channel);
  }
  channel->SetIndex(kNew);
}

void EPoller::Update(int operation, Channel* channel) {
  struct epoll_event event;
  bzero(&event, sizeof(event));
  event.events = channel->Events();
  event.data.ptr = channel;
  int fd = channel->Fd();
  if (::epoll_ctl(epollfd_,operation, fd, &event) < 0) {
    LOG_FATAL << "epoll_ctl op=" << operation << ", fd=" << fd;
  }
}

} // namespace simple_reactor
