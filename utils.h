#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <netinet/in.h>

#define IRC_MSG_SIZE 1024
#define IRC_SENDER_SIZE 128
#define IRC_BUFFER_SIZE 2048

typedef struct {
    struct sockaddr_in clientaddr;
    int clientfd;
    char senderName[IRC_SENDER_SIZE];
} ircClient;

typedef struct {
    uint32_t senderlen;
    uint32_t  messagelen;
    char sender[IRC_SENDER_SIZE];
    char message[IRC_MSG_SIZE];
} ircMessage;

// STRUCT CONSTRUCTORS
ircClient* Client();
ircMessage* Message();

void failOnError(const int, const char*);
int ircBroadcast(int*, int, const ircMessage, int);
int serializeMessage(const ircMessage*, char*, size_t);
ircMessage* deserializeMessage(const void*, const size_t);
void freeMessage(ircMessage*);

void printBufferHex(char*, uint32_t);

#endif
