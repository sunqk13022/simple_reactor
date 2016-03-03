#include "poller.h"

#include <poll.h>

#include <logging/logging.h>
#include "channel.h"
#include "event_loop.h"

namespace simple_reactor {

Poller::Poller(EventLoop* loop)
  : ownerloop_(loop)
{
}

Poller::~Poller() {
}

void Poller::FillActiveChannel(int numEvents, std::vector<Channel*>* channels) const {
  for (std::vector<struct pollfd>::const_iterator pfd = pollfds_.begin();
       pfd != pollfds_.end() && numEvents > 0; ++pfd) {
    if (pfd->revents <= 0) {
      continue;
    }
    --numEvents;

    std::map<int, Channel*>::const_iterator ch = channels_.find(pfd->fd);
    Channel* channel = ch->second;
    channel->SetReventsFd(pfd->revents);
    channels->push_back(channel);
  }
}

void Poller::UpdateChannel(Channel* channel) {
  ownerloop_->AssertInloopthread();
  LOG_INFO << "fd=" << channel->Fd() << " events=" << channel->Events();
  if (channel->Index() < 0) {
    struct pollfd pfd;
    pfd.fd = channel->Fd();
    pfd.events = static_cast<short>(channel->Events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
    int idx = static_cast<int>(pollfds_.size()) - 1;
    channel->SetIndex(idx);

    channels_[pfd.fd] = channel;
  } else {
    int idx = channel->Index();
    struct pollfd& pfd = pollfds_[idx];
    pfd.events = static_cast<short>(channel->Events());
    pfd.revents = 0;
    if (channel->IsNoneEvent()) {
      pfd.fd = -1;
    }
  }
}

Timestamp Poller::Poll(int timeout_micseconds, std::vector<Channel*>* active_channels) {
  int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeout_micseconds);
  Timestamp now = Timestamp::Now();

  if (numEvents > 0) {
    LOG_INFO << numEvents << " events happened";
    FillActiveChannel(numEvents, active_channels);
  } else if (numEvents == 0) {
    LOG_INFO << "nothing happened";
  } else {
    LOG_FATAL << "POLL::Poll()";
  }

  return now;
}

} // namespace simple_reactor
