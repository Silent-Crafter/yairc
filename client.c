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

    char clientName[IRC_BUFFER_SIZE];

    ircMessage* q = NULL;
    ircMessage* msg = Message();

    strncpy(hostname, argv[1], 20);
    port = atoi(argv[2]);

    sockfd = ircConnect(hostname, port);

    while((bytesReceived = recv(sockfd, buf, IRC_BUFFER_SIZE, 0))) {
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
                printf("%s: %s ",q->sender, q->message);
                scanf("%s", clientName);
                printf("[DEBUG] Sending %s as username with length %lu\n", clientName, strlen(clientName));
                msg->messageType = IRC_PK_AUTHA;
                msg->senderlen = strlen(clientName);
                msg->messagelen = msg->senderlen;
                strncpy(msg->sender, clientName, msg->senderlen);
                strncpy(msg->message, clientName, msg->senderlen);
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
                printf("[DEBUG] Received %s from server with length %lu\n", q->message, strlen(q->message));
                printf("%s: %s\n", q->sender, q->message);
                memset(buf, 0, IRC_BUFFER_SIZE);
                scanf("%s", buf);

                msg->messageType = IRC_PK_MSG;
                msg->senderlen = strlen(clientName);
                msg->messagelen = strlen(buf);

                strncpy(msg->sender, clientName, msg->senderlen);
                strncpy(msg->message, buf, msg->messagelen);
                memset(buf, 0, msg->senderlen+msg->messagelen);

                bytesSend = serializeMessage(msg, buf, IRC_BUFFER_SIZE);
                send(sockfd, buf, bytesSend, 0);
                memset(buf, 0, bytesSend+10);
                break;

            default:
                break;
        }
    }
    freeMessage(msg);
    freeMessage(q);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    return 0;
}
