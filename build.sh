#/bin/sh

mkdir -p bin/
gcc -Wall main.c utils.c -o bin/irc -g
