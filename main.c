#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "utils.h"

int main() {
    ircMessage* message = Message();
    ircMessage* messageOut;

    char sender[] = "Nikhil";
    char msg[] = "Hello World";

    char buffer[IRC_BUFFER_SIZE];

    message->senderlen = strlen(sender);
    message->messagelen = strlen(msg);
    strncpy(message->sender, sender, message->senderlen);
    strncpy(message->message, msg, message->messagelen);

    printf("\nSENDER_LEN=%u\nMESSAGE_LEN=%u\nSENDER=%s\nMESSAGE=%s\n", message->senderlen, message->messagelen, message->sender, message->message);

    int bytes = serializeMessage(message, buffer, IRC_BUFFER_SIZE - 1);
    buffer[bytes] = '\0';
    printBufferHex(buffer, bytes);
    messageOut = deserializeMessage(buffer, bytes);

    if (!messageOut) {
        perror("Error while deserializing");
        printf("\nerrno=%d\n", errno);
    } else {
        printf("\nSENDER_LEN=%u\nMESSAGE_LEN=%u\nSENDER=%s\nMESSAGE=%s\n", messageOut->senderlen, messageOut->messagelen, messageOut->sender, messageOut->message);
    }

    if (message)freeMessage(message);
    if (messageOut) freeMessage(messageOut);
    return 0;
}