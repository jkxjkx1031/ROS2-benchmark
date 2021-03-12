#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

const int PORT0 = 10080;
const int N_SAMPLE = 1000;
const int MAX_MSG_LEN = 20;
const char *GROUP_ADDR = "239.0.0.1";
long long write_latency[10000];

int main(int argc, char **argv)
{
    int n_srv, freq, sockfd, len;
    struct sockaddr_in addr;
    char buf[100];
    struct timeval tv;
    long long now, send;

    n_srv = atoi(argv[1]);
    freq = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(GROUP_ADDR);
    addr.sin_port = htons(PORT0);
    len = sizeof(addr);

    for (int i = 0; i < N_SAMPLE; i++)
    {
        bzero(buf, MAX_MSG_LEN);
        gettimeofday(&tv, NULL);
        send = tv.tv_sec * 1000000 + tv.tv_usec;
        sprintf(buf, "%lld", send);
        sendto(sockfd, buf, MAX_MSG_LEN, 0, (struct sockaddr*)&addr, len);
        // fprintf(stderr, "ready to print: %s\n", buf);
        usleep(1000000 / freq);
    }

    close(sockfd);

    for (int i = 0; i < N_SAMPLE; i++)
        printf("%lld\n", write_latency[i]);

    return 0;
}