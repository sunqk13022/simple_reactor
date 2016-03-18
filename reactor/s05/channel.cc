#include "channel.h"

#include <sstream>
#include <poll.h>

#include <logging/logging.h>
#include "event_loop.h"

namespace simple_reactor {

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
  : loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1)
{
}

Channel::~Channel() {
}

void Channel::Update() {
  loop_->UpdateChannel(this);
}

void Channel::EnableReading() {
  events_ |= kReadEvent;
  Update();
}

EventLoop* Channel::GetLoop() const {
  return loop_;
}

void Channel::HandleEvent() {
  if (revents_ & POLLNVAL) {
    LOG_WARN << "Channel::handle_event() POLLNVAL";
  }

  if (revents_ & (POLLERR | POLLNVAL)) {
    if (error_callback_) error_callback_();
  }

  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (read_callback_) read_callback_();
  }

  if (revents_ & POLLOUT) {
    if (write_callback_) write_callback_();
  }
}

void Channel::SetReadCallback(const EventCallback& cb) {
  read_callback_ = cb;
}

void Channel::SetWriteCallback(const EventCallback& cb) {
  write_callback_ = cb;
}

void Channel::SetErrorCallback(const EventCallback& cb) {
  error_callback_ = cb;
}

int Channel::Fd() const {
  return fd_;
}

int Channel::Events() const {
  return events_;
}

void Channel::SetReventsFd(int revt) {
  revents_ = revt;
}

int Channel::Index() const {
  return index_;
}

void Channel::SetIndex(int idx) {
  index_ = idx;
}

bool Channel::IsNoneEvent() const {
  return events_ == kNoneEvent;
}

} // namespace simple_reactor
