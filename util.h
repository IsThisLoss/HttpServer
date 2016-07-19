#ifndef HTTPSERVER_UTIL_H
#define HTTPSERVER_UTIL_H

#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

ssize_t sock_fd_write(int sock, void* buff, ssize_t bufflen, int fd);
ssize_t sock_fd_read(int sock, void* buff, ssize_t bufflen, int* fd);

int set_nonblock(int fd);

#endif //HTTPSERVER_UTIL_H
