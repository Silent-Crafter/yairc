#/bin/sh

mkdir -p bin/
gcc -std=c17 -Wall main.c utils.c -o bin/irc -g
gcc -std=c17 -Wall client.c utils.c -o bin/irc-client -g
