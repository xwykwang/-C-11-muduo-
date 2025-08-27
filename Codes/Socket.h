#pragma once

#include "noncopyable.h"
#include "InetAddress.h"

class Socket : noncopyable
{

public:
    explicit Socket(int sockfd)
    :sockfd_(sockfd)
    {}
    ~Socket();

    int fd() const { return sockfd_; }
    void bindAddress(const InetAddress &localAddr);
    void listen();
    int accept(InetAddress *peerAddr);

    void shutDownWrite();
    
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;

};