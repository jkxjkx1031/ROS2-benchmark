#include <arpa/inet.h>
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
const int N_SAMPLE = 8000;
const int MAX_MSG_LEN = 20;
double avg_rtt[100];

void *run_client(void *thread_id)
{
    struct sockaddr_in srv;
    int sockfd;

    bzero(&srv, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv.sin_port = htons(PORT0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (struct sockaddr*)&srv, sizeof(srv));

    char sendmsg[100] = "_hereis10B", recvmsg[100];
    struct timeval tv_start, tv_end;

    for (int i = 1; i <= N_SAMPLE; i++)
    {
        gettimeofday(&tv_start, NULL);
        write(sockfd, sendmsg, MAX_MSG_LEN);
        read(sockfd, recvmsg, MAX_MSG_LEN);
        gettimeofday(&tv_end, NULL);
        avg_rtt[(long long)thread_id] += tv_end.tv_sec * 1000000 + tv_end.tv_usec - tv_start.tv_sec * 1000000 - tv_start.tv_usec;
    }
    close(sockfd);
    avg_rtt[(long long)thread_id] /= N_SAMPLE;

    return NULL;
}

int main(int argc, char **argv)
{
    int n_cli = atoi(argv[1]);
    pthread_t thread[100];

    for (long long i = 0; i < n_cli; i++)
        pthread_create(&thread[i], NULL, run_client, (void*)i);
    for (long long i = 0; i < n_cli; i++)
        pthread_join(thread[i], NULL);

    double res = 0;
    for (int i = 0; i < n_cli; i++)
        res += avg_rtt[i];
    res /= n_cli;
    printf("%.6f\n", res);
    
    return 0;
}