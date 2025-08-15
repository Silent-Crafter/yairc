#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 1;
    }

    int port;
    int sockfd;
    int bytesReceived;
    int bytesSend;
    char hostname[20];
    char buf[IRC_BUFFER_SIZE];

    ircMessage* q = NULL;
    ircMessage* msg = Message();

    strncpy(hostname, argv[1], 20);
    port = atoi(argv[2]);

    sockfd = ircConnect(hostname, port);

    bytesReceived = recv(sockfd, buf, IRC_BUFFER_SIZE, 0);
    if (bytesReceived <= 0) {
        printf("[ERROR] Failed to receive data");
        close(sockfd);
        return 1;
    }

    q = deserializeMessage(buf, bytesReceived);
    if (!q) {
        perror("[ERROR] Unable to desserialize");
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        return 1;
    }

    switch (q->messageType) {
        case IRC_PK_AUTHQ:
            printf("%s ", q->message);
            scanf("%s", buf);
            printf("[DEBUG] Sending %s as username with length %lu\n", buf, strlen(buf));
            msg->messageType = IRC_PK_AUTHA;
            msg->senderlen = strlen(buf);
            msg->messagelen = msg->senderlen;
            strncpy(msg->sender, buf, msg->senderlen);
            strncpy(msg->message, buf, msg->senderlen);
            memset(buf, 0, msg->senderlen+5);
            bytesSend = serializeMessage(msg, buf, IRC_BUFFER_SIZE);
            send(sockfd, buf, bytesSend, 0);
            memset(buf, 0, bytesSend+10);
            break;

        case IRC_PK_AUTHA:
            break;

        case IRC_PK_CMD:
            break;

        case IRC_PK_MSG:
            break;

        default:
            break;
    }

    freeMessage(msg);
    freeMessage(q);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    return 0;
}
