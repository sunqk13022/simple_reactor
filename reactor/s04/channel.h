#ifndef SIMPLE_REACTOR_REACTOR_SO1_CHANNEL_H_
#define SIMPLE_REACTOR_REACTOR_SO1_CHANNEL_H_

#include <boost/function.hpp>


namespace simple_reactor {

class EventLoop;

class Channel {
 public:
  typedef boost::function<void ()> EventCallback;

  Channel(EventLoop* loop, int fd);
  ~Channel();

  void HandleEvent();

  void SetReadCallback(const EventCallback& cb);
  void SetWriteCallback(const EventCallback& cb);
  void SetErrorCallback(const EventCallback& cb);

  int Fd() const;
  int Events() const;

  void SetReventsFd(int revt);

  int Index() const;
  void SetIndex(int idx);
  bool IsNoneEvent() const;

  EventLoop* GetLoop() const;

  void EnableReading();

 private:

  void Update();

 private:

  EventLoop* loop_;
  const int  fd_;
  int        events_;
  int        revents_;
  int        index_;

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventCallback  read_callback_;
  EventCallback  write_callback_;
  EventCallback  error_callback_;
}; // class Channel

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_SO1_CHANNEL_H_
