#/bin/sh

mkdir -p bin/
cd bin/
gcc -Wall main.c utils.c -o irc -g
