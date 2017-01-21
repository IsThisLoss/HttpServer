#ifndef HTTPSERVER_UTIL_H
#define HTTPSERVER_UTIL_H

#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * @param sock is socket which get message
 * @param buff is a content to pass
 * @param bufflen is a size of content
 * @param fd is descriptor which will be passed
 * @return size of written message
 *
 * Warning! It throws exceptions
 */
ssize_t sock_fd_write(int sock, void* buff, ssize_t bufflen, int fd);

/**
 * @param sock is socket which get message
 * @param buff is a content to accept
 * @param bufflen is a size of content
 * @param fd is descriptor which will be accepted
 * @return size of read message
 *
 * Warning! It throws exceptions
 */
ssize_t sock_fd_read(int sock, void* buff, ssize_t bufflen, int* fd);

/**
 * @brief set socket to non-block mode
 * @param fd descriptor which will ne non-block
 * @return operation's status
 */
int set_nonblock(int fd);

#endif //HTTPSERVER_UTIL_H
