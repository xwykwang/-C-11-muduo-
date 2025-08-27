#include "Acceptor.h"
#include "Logger.h"

#include <errno.h>
#include <unistd.h>

static int createNonblocking()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(sockfd < 0)
    {
        LOG_FATAL("%s:%s:%d listen socket create error:%d", __FILE__, __FUNCTION__, __LINE__, errno);
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort)
    :loop_(loop),
    acceptorSocket_(createNonblocking()),
    acceptorChannel_(loop, acceptorSocket_.fd()),
    listenning_(false)
{
    acceptorSocket_.setReuseAddr(true);
    acceptorSocket_.setReusePort(reusePort);
    acceptorSocket_.bindAddress(listenAddr);
    acceptorChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    acceptorChannel_.disableAll();
    acceptorChannel_.remove();
}

void Acceptor::listen()
{
    listenning_ = true;
    acceptorSocket_.listen();
    acceptorChannel_.enableReading();//epoll_add
}

void Acceptor::handleRead() // 没有被调用
{
    InetAddress peerAddr;
    int connfd = acceptorSocket_.accept(&peerAddr);
    LOG_INFO("handle acceptor read: acceptor")
    if(connfd > 0)
    {
        if(newConnectionCallback_)
        {
            newConnectionCallback_(connfd, peerAddr);
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        LOG_ERROR("%s:%s:%d accept error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        if(errno == EMFILE)
        {
            LOG_ERROR("%s:%s:%d sockfd reached limit\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}