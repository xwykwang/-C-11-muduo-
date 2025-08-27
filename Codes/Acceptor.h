#pragma once

#include "noncopyable.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"

#include <functional>

class Acceptor : noncopyable
{

public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb)
    {
        newConnectionCallback_ = cb;
    }

    bool listenning() const { return listenning_; }
    void listen();


private:
    void handleRead();
    
    EventLoop *loop_;
    Socket acceptorSocket_;
    Channel acceptorChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};
