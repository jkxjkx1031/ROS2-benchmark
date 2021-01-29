#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int sockfd, connfd;
    struct sockaddr_in addr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 5);

    fprintf(stderr, "waiting for client message...\n");

    int n, len;
    char buf[100], *ep;
    struct timeval tv;
    long long now, send;

    connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
    for (int i = 1; i <= 2000; i++)
    {
        read(connfd, buf, 20);
        // fprintf(stderr, "receive: %s\n", buf);
        send = strtoll(buf, &ep, 10);
        gettimeofday(&tv, NULL);
        now = tv.tv_sec * 1000000 + tv.tv_usec;
        fprintf(stdout, "%lld\n", now - send);
    }
    close(sockfd);

    return 0;
}