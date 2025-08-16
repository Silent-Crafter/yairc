#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

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
    char buf[IRC_BUFFER_SIZE] = {0};
    int bufSize = 0;

    char clientName[IRC_BUFFER_SIZE] = {0};
    int clientNameSize = 0;

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
            continue;
        }

        switch (q->messageType) {
            case IRC_PK_AUTHQ:
                printf("%s ",q->message);
                if ((clientNameSize = input(&(char*){clientName}, IRC_BUFFER_SIZE, 1)) < 0) {
                    perror("[ERROR] Input error");
                    break;
                }
                printf("[DEBUG] Sending %s as username with length %lu\n", clientName, strlen(clientName));
                ircSendMessage(sockfd, clientName, clientName, IRC_PK_AUTHA, clientNameSize, clientNameSize);

                // Reset buffer
                memset(buf, 0, IRC_MSG_SIZE);
                break;

            case IRC_PK_AUTHA:
                break;

            case IRC_PK_CMD:
                break;

            case IRC_PK_MSG:
                printf("[DEBUG] Received %s from server with length %lu\n", q->message, strlen(q->message));
                printf("%s: %s\n", q->sender, q->message);
                memset(buf, 0, IRC_BUFFER_SIZE);
                printf("\n%s: ", clientName);
                bufSize = input(&(char*){buf}, IRC_BUFFER_SIZE, 1);
                if (bufSize < 0) {
                    perror("[ERROR] Input error");
                    break;
                }

                if (bufSize == 0) {
                     strncpy(buf, " ", 0);
                }

                ircSendMessage(
                    sockfd,
                    buf,
                    clientName,
                    IRC_PK_MSG,
                    bufSize,
                    clientNameSize
                );
                memset(buf, 0, IRC_BUFFER_SIZE);
                break;

            default:
                break;
        }
    }
    if (msg) freeMessage(msg);
    if (q) freeMessage(q);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    return 0;
}
