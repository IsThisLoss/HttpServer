#ifndef HTTPSERVER_ACCEPTOR_H
#define HTTPSERVER_ACCEPTOR_H


#include <algorithm>
#include <map>
#include <list>
#include <fstream>

#include <ev++.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "util.h"

extern std::ofstream logs;

/**
 * @class Acceptor
 *
 * @brief accept new connections
 *
 * It provides accept functionality
 * It runs in single process
 * and pass client's sockets in other helper's process
 */
class Acceptor final
{
private:
    /**
     * @brief list of client's watchers
     */
    std::list<ev::io*> watchers;

    /**
     * @brief watcher for accept new incoming connection
     */
    ev::io* accept_watcher;

    /**
     * @brief watcher to keep connection with crash handler process
     */
    ev::io* crash_watcher;

    /**
     * @brief map from helper's descriptor to number of clients
     * which currently connected to this helper
     */
    std::map<int, int> helpers; // fd -> num of clients

    /**
     * @brief callback for @ref crash_watcher which add new helper process if it needs
     */
    void new_helper(ev::io& watcher, int);

    /**
     * @brief callback for @ref acceptor_watcher which handles new connections
     *
     * It pass client's socket to the least busy @ref Helper and
     * all the rest interaction occur only in helper
     */
    void new_connection(ev::io& watcher, int);

    /**
     * @brief callback for @ref watchers, it's update helpers workload
     * when client disconnect
     */
    void read_from_helper(ev::io& watcher, int);

public:

    /**
     * @param fd is a socket to crash handler process
     * @param ip is an ip address of server
     * @param port is a port which will be listened
     */
    Acceptor(int fd, const std::string& ip, const int& port);
    ~Acceptor();
};


#endif //HTTPSERVER_ACCEPTOR_H
