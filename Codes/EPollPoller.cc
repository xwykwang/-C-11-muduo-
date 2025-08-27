#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

//channel has not be added
const int kNew = -1;
//channel has been added
const int kAdded = 1;
//channel has beed deleted
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop)
    :Poller(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{
    if(epollfd_ < 0)
    {
        LOG_FATAL("epoll_create error:%d\n", errno);
    }
}

EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

void EPollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index();
    LOG_INFO("func = %s fd = %d events = %d index = %d \n", __FUNCTION__, channel->fd(), channel->events(), channel->index());
    if(index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if(index == kNew)
        {
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        int fd = channel->fd();
        if(channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel)
{
    LOG_INFO("func = %s fd = %d events = %d index = %d \n", __FUNCTION__, channel->fd(), channel->events(), channel->index());
    int fd = channel->fd();
    channels_.erase(fd);

    int index = channel->index();
    if(index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPollPoller::update(int operation, Channel *channel)
{
    epoll_event event;
    bzero(&event, sizeof event);
    int fd = channel->fd();

    event.events = channel->events();
    event.data.ptr = channel;
    // event.data.fd = fd;

    // LOG_INFO("update channel: %p", channel);
    // LOG_INFO("update channel fd: %d", fd);
    // LOG_INFO("test event channel: %p", event.data.ptr);
    // LOG_INFO("test event fd: %d", event.data.fd);
    // [INFO]: 20250716 01:03:36 update channel: 0x560c31a1c8d0
    // [INFO]: 20250716 01:03:36 update channel fd: 5
    // [INFO]: 20250716 01:03:36 test event channel: 0x560c00000005
    // [INFO]: 20250716 01:03:36 test event fd: 5
    // 先给ptr赋值后给fd赋值会导致ptr部分值被fd覆盖
    // 原因: epoll_data_t是一个union结构，fd和ptr指针只能使用一个

    if(::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if(operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("EPOLL_CTL_DEL error:%d\n", errno);
        }
        else
        {
            LOG_FATAL("EPOLL_CTL_ADD/MOD error:%d\n", errno);
        }
    }
}


Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    LOG_INFO("func = %s, total fd:%d\n", __FUNCTION__, static_cast<int>(channels_.size()));

    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if(numEvents > 0)
    {
        LOG_INFO("%d events happened\n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if(numEvents == 0)
    {   
        LOG_INFO("no event happen, %s time out\n", __FUNCTION__);
    }
    else
    {
        if(saveErrno == EINTR)
        {
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll() error");
        }
    }
    return now;
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activechannels) const
{
    LOG_INFO("started to fill %d events in vector:%d", numEvents,(int)events_.size());
    for(int i = 0; i < numEvents; ++i)
    {
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        //Segmentation fault (core dumped) in channel->set_revent() : the channel maby invalied
        channel->set_revent(events_[i].events);
        LOG_INFO("push active channels");
        activechannels->push_back(channel);
    }
    LOG_INFO("filled %d events", numEvents);
}









