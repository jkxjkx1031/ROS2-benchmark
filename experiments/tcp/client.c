#include <arpa/inet.h>
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
    struct sockaddr_in srv;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&srv, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv.sin_port = htons(8080);

    char buf[100];
    struct timeval tv;
    long long now;

    connect(sockfd, (struct sockaddr*)&srv, sizeof(srv));
    for (int i = 1; i <= 2000; i++)
    {
        bzero(buf, 20);
        gettimeofday(&tv, NULL);
        now = tv.tv_sec * 1000000 + tv.tv_usec;
        sprintf(buf, "%lld", now);
        write(sockfd, buf, 20);
        fprintf(stderr, "ready to print: %s\n", buf);
    }
    close(sockfd);

    return 0;
}