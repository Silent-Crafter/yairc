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

    // TODO: Implement conversation between multiple clients
    //        instead of server and client
    clients[0] = acceptClient(sockfd);
    printf("[INFO] Received User name: %s\n", clients[0]->senderName);

    while (1) {
        char buf[IRC_BUFFER_SIZE];
        printf("\nSERVER: ");
        fflush(stdout);
        scanf("%s", buf);

        if (!strncmp(buf, "/bye", 4)) break;

        ircSendMessage(clients[0], sockfd, buf, "SERVER", IRC_PK_MSG, strlen(buf), 6);
        memset(buf, 0, IRC_BUFFER_SIZE);

        int bytesReceived = recv(clients[0]->clientfd, buf, IRC_BUFFER_SIZE, 0);

        if (bytesReceived <= 0) {
            break;
        }

        printf("[INFO] Bytes Received: %d\n", bytesReceived);
        ircMessage* msg = deserializeMessage(buf, bytesReceived);
        printf("\n%s: %s", msg->sender, msg->message);
        fflush(stdout);
    }

    freeClient(clients[0]);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    return 0;
}
