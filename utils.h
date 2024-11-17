#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <unistd.h>

#define IRC_MSG_SIZE 1024
#define IRC_SENDER_SIZE 128
#define IRC_BUFFER_SIZE 2048
#define IRC_MAX_CLIENTS 100

typedef enum { IRC_PK_AUTHQ, IRC_PK_AUTHA, IRC_PK_CMD, IRC_PK_MSG } message_t;


typedef struct {
    struct sockaddr_in clientaddr;
    socklen_t clientlen;
    int clientfd;
    char senderName[IRC_SENDER_SIZE];
} ircClient;

typedef struct {
    uint32_t senderlen;
    uint32_t  messagelen;
    message_t messageType;
    char sender[IRC_SENDER_SIZE];
    char message[IRC_MSG_SIZE];
} ircMessage;


// ==============================
// STRUCT CONSTRUCTORS
// ==============================
ircClient* Client();
ircMessage* Message();


// ==============================
// MESSAGING FUNCTIONS
// ==============================
int ircBroadcast(int*, int, const ircMessage, int);
int serializeMessage(const ircMessage*, char*, size_t);
ircMessage* deserializeMessage(void*, const size_t);
void freeMessage(ircMessage*);


// ==============================
// CLIENT-SERVER FUNCTIONS
// ==============================
int initServer(const char*, int, struct sockaddr_in*, socklen_t*);
ircClient* acceptClient(int);
void clientHandler(void*);
void removeClient(ircClient*, ircClient**, int*);
void freeClient(ircClient*);


// ==============================
// MISC FUNCTIONS
// ==============================
int failOnError(const int, const char*);
void printBufferHex(char*, uint32_t);

#endif
