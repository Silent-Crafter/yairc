#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>

#include "utils.h"

typedef struct sockaddr SA;

void printBufferHex(char* buffer, uint32_t size) {
    printf("\n----------BUFFER----------\n");
    int i = 0;
    int j = 0;
    int k = 0;
    while (i != size) {
        if (j == 4) {
            printf(" ");
            k++;
            j = 0;
        }

        if (k == 4) {
            printf("\n");
            k = 0;
        }
        printf("%.2x ", buffer[i]);
        j++;
        i++;
    }
    printf("\n--------------------------\n");
}

/*
 * Default Constructor for ircMessage
 * @param none
 * @return object of type ircMessage
 */
 ircMessage* Message() {
     ircMessage *msg = (ircMessage*)malloc(sizeof(ircMessage));
     memset(msg, 0, sizeof(ircMessage));
     return msg;
 }

 /*
 * Default Constructor for ircClient
 * @param none
 * @return object of type ircClient
 */
 ircClient* Client() {
     ircClient* client = (ircClient*)malloc(sizeof(ircClient));
     memset(client, 0, sizeof(ircClient));
     client->clientlen = sizeof(SA);
     return client;
 }

/*
 * failOnError function exits the program if an error occurs during socket function calls
 * @param retval: return value of any socket function
 * @param errorMessage: error message to print before quiting
 * @return the original return value
 */
int failOnError(const int retval, const char* errorMessage) {
    // most socket functions fail by denoting a negative return value
    if (retval >= 0) return retval;
    perror(errorMessage);
    printf("\n");
    exit(EXIT_FAILURE);

    // Unreachable
    return 1;
}

/*
 * ircBoradcast() is used to send a message to all connected clients
 * @param msg: a structure message containing the sender name and the message itself
 *              along with their respective string lengths
 * @param flags: flags for the send() function
 */
int ircBroadcast(int* clients, int clientSize, const ircMessage msg, int flags) {
    for (int i = 0 ; i < clientSize ; i++) {
        ;
    }

    return 0;
}

/*
 * serializeMessage() takes a ircMessage object and packs its members in a dest char array
 * with size dsize. if data is less than dsize it will padd the destination will null characters
 * @param src: ircMessage object
 * @param dest: buffer to store the serialized structure
 * @param dsize: size of the destination buffer
 * @return int actual size written in buffer
 */
int serializeMessage(const ircMessage* src, char* dest, size_t dsize) {
    if (!src) return -1;

    int actualsize = 2 * sizeof(uint32_t) + sizeof(message_t) + strlen(src->message) + strlen(src->sender);

    // Message too big for buffer
    if (actualsize < 1 || actualsize > dsize) {
        errno = ERANGE;
        return -1;
    }

    int remainingBytes = dsize - actualsize;

    // Convert from host byte order to network byte order
    // to be compatible with cpus having different endianess
    size_t senderlen = htonl(src->senderlen);
    size_t messagelen = htonl(src->messagelen);

    // Copy data to dest buffer member by member
    memcpy(dest, &src->messageType, sizeof(message_t));
    dest += sizeof(message_t);

    memcpy(dest, &senderlen, sizeof(uint32_t));
    dest += sizeof(uint32_t);

    memcpy(dest, &messagelen, sizeof(uint32_t));
    dest += sizeof(uint32_t);

    memcpy(dest, src->sender, src->senderlen);
    dest += src->senderlen;

    memcpy(dest, src->message, src->messagelen);
    dest += src->messagelen;

    // Pad the remaining bytes with null character
    if (remainingBytes > 0) memset(dest, 0, remainingBytes);

    return actualsize;
}

/*
 * Unpack the message buffer and create a ircMessage object with it
 * @param src: the message buffer
 * @param ssize: size of the message buffer
 * @return object of type ircMessage
 */
ircMessage* deserializeMessage(void* buf, const size_t ssize) {
    if (buf == NULL) {
        return NULL;
    }

    // Check for the least possible lower bound that message buffer can have
    if (ssize < 2*sizeof(uint32_t) + sizeof(message_t)) {
        errno = EBADE;
        return NULL;
    }

    size_t senderlen = 0, messagelen = 0;
    message_t messageType;
    char message[IRC_MSG_SIZE] = {0};
    char sender[IRC_SENDER_SIZE] = {0};
    void* src = buf;

    memcpy(&messageType, src, sizeof(message_t));
    src += sizeof(message_t);

    memcpy(&senderlen, src, sizeof(uint32_t));
    src += sizeof(uint32_t);

    memcpy(&messagelen, src, sizeof(uint32_t));
    src += sizeof(uint32_t);

    // Convert from network byte order to host byte order
    // to handle differnt endianess of data
    senderlen = ntohl(senderlen);
    messagelen = ntohl(messagelen);

    // Sussy activity
    if ((messagelen + senderlen + 2*sizeof(uint32_t) + sizeof(message_t)) != ssize) {
        errno = EBADE;
        return NULL;
    }

    // Boundry check to prevent malformed or corrupted packet
    if (!(senderlen >= 1 && senderlen < IRC_SENDER_SIZE) || !(messagelen >= 1 && messagelen < IRC_MSG_SIZE)) {
        errno = ERANGE;
        return NULL;
    }

    strncpy(sender, src, senderlen);
    sender[senderlen] = '\0';
    src += senderlen;

    strncpy(message, src, messagelen);
    message[messagelen] = '\0';

    // Pack the data in the message structure and return it
    ircMessage* msg = Message();
    msg->messageType = messageType;
    msg->senderlen = strlen(sender);
    msg->messagelen = strlen(message);
    // use memcpy instead of strncpy as the message is already null terminated
    // as strncpy only copies non null bytes
    memcpy(msg->sender, sender, msg->senderlen);
    memcpy(msg->message, message, msg->messagelen);

    // Flush the buffer
    memset(buf, 0, ssize);

    return msg;
}

// free memory safely
void freeMessage(ircMessage* msg) {
    if (msg != NULL) {
        free(msg);
    }
}

void freeClient(ircClient* client) {
    if (client != NULL) {
        if(client->clientfd) {
            shutdown(client->clientfd, SHUT_RDWR);
            close(client->clientfd);
        }
        free(client);
    }
}

int initServer(const char* hostname, int port, struct sockaddr_in* servaddr, socklen_t* servlen) {
    int sockfd;
    int opt = 1;

    servaddr->sin_family = AF_INET;
    servaddr->sin_port = htons(port);
    inet_pton(AF_INET, hostname, &servaddr->sin_addr);

    *servlen = sizeof(struct sockaddr_in);

    sockfd = failOnError(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), "[ERROR] Socket creation failed");
    failOnError(bind(sockfd, (struct sockaddr*)servaddr, *servlen), "[ERROR] bind() failed");
    failOnError(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE | SO_REUSEADDR, &opt, sizeof(int)), "[ERROR] setsockopt() failed");
    failOnError(listen(sockfd, IRC_MAX_CLIENTS), "[ERROR] listen() failed");

    return sockfd;
}

ircClient* acceptClient(int sockfd) {
    ircClient* client = Client();

    client->clientfd = failOnError(accept(sockfd, (SA*)&(client->clientaddr), &(client->clientlen)), "[ERROR] Failed to accept connection");
    printf("[INFO] connection from %s:%d at fd %d\n", inet_ntoa(client->clientaddr.sin_addr), ntohs(client->clientaddr.sin_port), client->clientfd);

    // TODO: REIMPLEMENT WITH PROPER HEADERS AND AUTHENTICATION PROTOCOL
    ircMessage* q = Message();
    q->messageType = IRC_PK_AUTHQ;
    q->senderlen = 6;
    q->messagelen = 5;
    strncpy(q->sender, "SERVER", q->senderlen);
    strncpy(q->message, "USER:", q->messagelen);

    char buf[IRC_BUFFER_SIZE];
    int bytes = serializeMessage(q, buf, IRC_BUFFER_SIZE);
    send(client->clientfd, buf, bytes, 0);

    memset(buf, 0, IRC_BUFFER_SIZE);
    int bytesReceived = recv(client->clientfd, buf, IRC_BUFFER_SIZE, 0);
    ircMessage* ans = deserializeMessage(buf, bytesReceived);

    if (!ans) {
        perror("Error occured");
        freeClient(client);
        return NULL;
    }

    strncpy(client->senderName, ans->message, ans->messagelen);
    printf("TYPE=%d\nSENDER=%s\nDATA=%s\n", ans->messageType, ans->sender, ans->sender);

    return client;
}

void removeClient(ircClient* client, ircClient* clients[], int* totalClients) {
    if (client == NULL) return;
    char* hostname = inet_ntoa(client->clientaddr.sin_addr);
    int port = ntohs(client->clientaddr.sin_port);

    int i;
    for (i = 0 ; i < *totalClients ; i++) {
        if (clients[i]->clientfd == client->clientfd) {
            freeClient(clients[i]);
            clients[i] = NULL;
            break;
        }
    }

    clients[i] = clients[(*totalClients)-1];
    clients[(*totalClients)-1] = NULL;
    (*totalClients) -= 1;
    printf("[INFO] Successfully closed connection from %s:%d\n", hostname, port);
}
