#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

const int PORT0 = 10080;
const int MAX_MSG_LEN = 20;

int main(int argc, char **argv)
{
    int epollfd = epoll_create1(0);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0), connfd;
    struct epoll_event ev, events[100];
    struct sockaddr_in addr, cli;
    int ready, len;
    char sendmsg[100] = "ACKfromSrv", recvmsg[100];

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 20);

    ev.data.fd = sockfd;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);

    for (;;)
    {
        ready = epoll_wait(epollfd, events, 100, 5000);
        if (ready <= 0) break;
        for (int i = 0; i < ready; i++)
            if (events[i].data.fd == sockfd)
            {
                if (events[i].events & EPOLLIN)
                {
                    len = sizeof(cli);
                    connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
                    ev.data.fd = connfd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);
                }
            }
            else
            {
                if (events[i].events & EPOLLIN)
                {
                    if (read(events[i].data.fd, recvmsg, MAX_MSG_LEN) <= 0)
                    {
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        continue;
                    }
                    write(events[i].data.fd, sendmsg, MAX_MSG_LEN);
                }
                else if (events[i].events & EPOLLRDHUP)
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            }
    }
    close(sockfd);

    return 0;
}