#pragma once

#include "Timestamp.h"
#include "noncopyable.h"

#include <functional>
#include <memory>

class EventLoop;

class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop * loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);

    //set callback function
    void setReadCallback(ReadEventCallback cb){ readCallback_ = std::move(cb);}
    void setWriteCallback(EventCallback cb){ writeCallback_ = std::move(cb);}
    void setCloseCallback(EventCallback cb){ closeCallback_ = std::move(cb);}
    void setErrorCallback(EventCallback cb){ errorCallback_ = std::move(cb);}

    int fd() const {return fd_;}
    int events() const {return events_;}
    void set_revent(int revt) {revents_ = revt;}
    int index() { return index_; }
    void set_index(int index) { index_ = index; }
    
    //set event status
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    bool isNoneEvent() const {return events_ == kNoneEvent;}
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    EventLoop* ownerLoop() { return loop_; }
    void tie(const std::shared_ptr<void>&);
    void remove();

private:

    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_; 
};