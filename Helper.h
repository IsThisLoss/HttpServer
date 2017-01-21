#ifndef HTTPSERVER_HELPER_H
#define HTTPSERVER_HELPER_H


#include <algorithm>
#include <vector>

#include <ev++.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <fstream>

#include "util.h"
#include "Responser.h"

extern std::ofstream logs;


/**
 * @class Helper
 *
 * @brief functionality to work with connected clients
 *
 * Helper is class which provide functions need to interact with
 * already connected clients
 */
class Helper final
{
private:
    /**
     * @brief object which generate content to response
     */
    Responser resp;

    /**
     * @brief watcher for @ref Acceptor process
     */
    ev::io* acceptor_watcher;

    /**
     * @brief vector of watchers of connected users
     */
    std::vector<ev::io*> watchers;

    /**
     * @brief callback for @ref acceptor_watcher which adds new clients
     */
    void read_from_acceptor(ev::io& watcher, int);

    /**
     * @brief callback which handel clients incoming messages
     */
    void read_from_client(ev::io& watcher, int);
public:
    /**
     * @param fd descriptor of socket connected with @ref Acceptor
     */
    Helper(int fd);
    ~Helper();
};


#endif //HTTPSERVER_HELPER_H
