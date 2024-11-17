#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        return 1;
    }

    char hostname[20];
    strncpy(hostname, argv[1], 20);

    int port = atoi(argv[2]);

    int sockfd, clientfd;
    int opt = 1;
    struct sockaddr_in servaddr;
    socklen_t servlen = sizeof(servaddr);

    ircClient* clients[IRC_MAX_CLIENTS];
    int totalClients = 0;

    sockfd = initServer(hostname, port, &servaddr, &servlen);

    printf("[INFO] Server listening on %s:%d\n", hostname, port);

    freeClient(acceptClient(sockfd));

    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    return 0;
}