#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "utils.h"

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
     msg->senderlen = 0;
     msg->messagelen = 0;
     memset(msg->sender, 0, IRC_SENDER_SIZE);
     memset(msg->message, 0, IRC_MSG_SIZE);
     return msg;
 }

/*
 * failOnError function exits the program if an error occurs during socket function calls
 * @param retval: return value of any socket function
 * @param errorMessage: error message to print before quiting
 */
void failOnError(const int retval, const char* errorMessage) {
    // most socket functions fail by denoting a negative return value
    if (retval >= 0) return;
    perror(errorMessage);
    exit(EXIT_FAILURE);
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

int serializeMessage(const ircMessage* src, char* dest, size_t dsize) {
    if (!src) return -1;

    int actualsize = 2 * sizeof(uint32_t) + src->messagelen + src->senderlen;

    // Message too big for buffer
    if (actualsize < 1 || actualsize > dsize) {
        errno = ERANGE;
        return -1;
    }

    // Convert from host byte order to network byte order
    // to be compatible with cpus having different endianess
    size_t senderlen = htonl(src->senderlen);
    size_t messagelen = htonl(src->messagelen);

    memcpy(dest, &senderlen, sizeof(uint32_t));
    memcpy(dest+sizeof(uint32_t), &messagelen, sizeof(uint32_t));
    memcpy(dest+2*sizeof(uint32_t), src->sender, src->senderlen);
    memcpy(dest+2*sizeof(uint32_t)+src->senderlen, src->message, src->messagelen);

    return actualsize;
}

/*
 * Unpack the message buffer and create a ircMessage object with it
 * @param src: the message buffer
 * @param ssize: size of the message buffer
 * @return object of type ircMessage
 */
ircMessage* deserializeMessage(const void* src, const size_t ssize) {
    if (src == NULL) {
        return NULL;
    }

    // Check for the least possible lower bound that message buffer can have
    if (ssize < 2*sizeof(uint32_t)) {
        return NULL;
    }

    size_t senderlen, messagelen;
    char message[IRC_MSG_SIZE], sender[IRC_SENDER_SIZE];

    memcpy(&senderlen, src, sizeof(uint32_t));
    src += sizeof(uint32_t);

    memcpy(&messagelen, src, sizeof(uint32_t));
    src += sizeof(uint32_t);

    // Convert from network byte order to host byte order
    // to handle differnt endianess of data
    senderlen = ntohl(senderlen);
    messagelen = ntohl(messagelen);

    // Sussy activity
    if ((messagelen + senderlen + 2*sizeof(uint32_t)) != ssize) {
        return NULL;
    }

    // Boundry check to prevent malformed or corrupted packet
    if (!(senderlen >= 1 && senderlen < IRC_SENDER_SIZE) || !(messagelen >= 1 && messagelen < IRC_MSG_SIZE)) {
        return NULL;
    }

    strncpy(sender, src, senderlen);
    sender[senderlen] = '\0';
    src += senderlen;

    strncpy(message, src, messagelen);
    message[messagelen] = '\0';

    // Pack the data in the message structure and return it
    ircMessage* msg = Message();
    msg->senderlen = strlen(sender);
    msg->messagelen = strlen(message);
    // use memcpy instead of strncpy as the message is already null terminated
    // as strncpy only copies non null bytes
    memcpy(msg->sender, sender, msg->senderlen);
    memcpy(msg->message, message, msg->messagelen);

    return msg;
}

void freeMessage(ircMessage* msg) {
    if (msg != NULL) {
        free(msg);
    }
}
