#ifndef HTTPSERVER_HELPER_H
#define HTTPSERVER_HELPER_H


#include <algorithm>
#include <vector>

#include <ev++.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#include "util.h"
#include "responser.h"


class helper final
{
private:
    responser resp;
    ev::dynamic_loop& loop;
    ev::io* acceptor_watcher;
    std::vector<ev::io*> watchers;
    void read_from_acceptor(ev::io& watcher, int);
    void read_from_client(ev::io& watcher, int);
public:
    helper(ev::dynamic_loop& _loop, int fd);
    ~helper();
};


#endif //HTTPSERVER_HELPER_H
