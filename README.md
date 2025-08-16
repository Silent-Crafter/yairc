# YAIRC

A C library that can be used to implement a secure internet relay chat application

## USAGE

Currently, all the library functions are defined in `utils.c` & `utils.h` and the implementation for server and client can be found in the files `main.c` and `client.c`.

1. Build the project
```sh 
mkdir -p bin
chmod +x ./build.sh
./build.sh
```

2. Launch server
```sh
./bin/irc <ip> <port>
```

3. Launch client
```sh
./bin/irc-client <ip> <port>
```
