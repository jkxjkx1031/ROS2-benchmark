#include <netinet/in.h>
#include <pthread.h>
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
long long latency[10000][100];

void *run_server(void *port)
{
    int sockfd, connfd;
    struct sockaddr_in addr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((long long)port);
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 5);

    fprintf(stderr, "port %lld - waiting for client message...\n", (long long)port);

    int n, len;
    char buf[100], *ep;
    struct timeval tv;
    long long now, send;

    connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
    for (int i = 1; i <= N_SAMPLE; i++)
    {
        read(connfd, buf, MAX_MSG_LEN);
        // fprintf(stderr, "receive: %s\n", buf);
        send = strtoll(buf, &ep, 10);
        gettimeofday(&tv, NULL);
        now = tv.tv_sec * 1000000 + tv.tv_usec;
        latency[i][(long long)port - PORT0] = now - send;
        // fprintf(stdout, "%lld\n", now - send);
    }
    close(sockfd);

    return NULL;
}

int main(int argc, char **argv)
{
    int n_srv;
    pthread_t thread[100];

    n_srv = atoi(argv[1]);
    for (long long i = 0; i < n_srv; i++)
        pthread_create(&thread[i], NULL, run_server, (void*)(i + PORT0));
    for (long long i = 0; i < n_srv; i++)
        pthread_join(thread[i], NULL);

    // double avg_min = 0, avg_max = 0;
    double result = 0;
    for (int i = 1; i <= N_SAMPLE; i++)
    {
        // long long min = 0x3f3f3f3f3f3f3f3f, max = 0;
        // for (int j = 0; j < n_srv; j++)
        // {
        //     if (latency[i][j] < min)
        //         min = latency[i][j];
        //     if (latency[i][j] > max)
        //         max = latency[i][j];
        // }
        // avg_min += (min - avg_min) / i;
        // avg_max += (max - avg_max) / i;
        for (int j = 0; j < n_srv; j++)
            result += latency[i][j];
    }
    // fprintf(stdout, "%.6f\n%.6f\n", avg_min, avg_max);
    result /= N_SAMPLE * n_srv;
    fprintf(stdout, "%.6f\n", result);

    return 0;
}