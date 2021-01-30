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
const int N_SAMPLE = 8000;
const int MAX_MSG_LEN = 20;

int main(int argc, char **argv)
{
    int n_srv, sockfd[100];
    struct sockaddr_in srv[100];

    n_srv = atoi(argv[1]);
    bzero(srv, sizeof(srv));
    for (int i = 0; i < n_srv; i++)
    {
        srv[i].sin_family = AF_INET;
        srv[i].sin_addr.s_addr = inet_addr("127.0.0.1");
        srv[i].sin_port = htons(PORT0 + i);
        sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);
        connect(sockfd[i], (struct sockaddr*)&srv[i], sizeof(srv[i]));
    }

    char buf[100];
    struct timeval tv;
    long long now;

    for (int i = 1; i <= N_SAMPLE; i++)
    {
        bzero(buf, MAX_MSG_LEN);
        gettimeofday(&tv, NULL);
        now = tv.tv_sec * 1000000 + tv.tv_usec;
        sprintf(buf, "%lld", now);
        for (int j = 0; j < n_srv; j++)
            write(sockfd[j], buf, MAX_MSG_LEN);
        // fprintf(stderr, "ready to print: %s\n", buf);
    }
    for (int i = 0; i < n_srv; i++)
        close(sockfd[i]);

    return 0;
}