#ifndef SIMPLE_REACTOR_REACTOR_SO1_CHANNEL_H_
#define SIMPLE_REACTOR_REACTOR_SO1_CHANNEL_H_

#include <boost/function.hpp>

#include "time/timestamp.h"

namespace simple_reactor {

class EventLoop;

class Channel {
 public:
  typedef boost::function<void ()> EventCallback;
  typedef boost::function<void (Timestamp)> ReadEventCallback;

  Channel(EventLoop* loop, int fd);
  ~Channel();

  void HandleEvent(Timestamp receiveTime);

  void SetReadCallback(const ReadEventCallback& cb);
  void SetWriteCallback(const EventCallback& cb);
  void SetErrorCallback(const EventCallback& cb);
  void SetCloseCallback(const EventCallback& cb);

  int Fd() const;
  int Events() const;

  void SetReventsFd(int revt);

  int Index() const;
  void SetIndex(int idx);
  bool IsNoneEvent() const;

  EventLoop* GetLoop() const;

  void EnableReading();
  void DisableAll();

  void EnableWriting();
  void DisableWriting();
  bool IsWriting() const;

 private:

  void Update();

 private:

  EventLoop* loop_;
  const int  fd_;
  int        events_;
  int        revents_;
  int        index_;
  bool       event_handling_;

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  ReadEventCallback  read_callback_;
  EventCallback  write_callback_;
  EventCallback  error_callback_;
  EventCallback  close_callback_;
}; // class Channel

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_SO1_CHANNEL_H_
