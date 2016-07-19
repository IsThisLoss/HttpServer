#ifndef HTTPSERVER_ACCEPTOR_H
#define HTTPSERVER_ACCEPTOR_H


#include <algorithm>
#include <map>
#include <vector>

#include <ev++.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "util.h"

class acceptor final
{
private:
    ev::dynamic_loop& loop;
    std::vector<ev::io*> watchers;
    ev::io* accept_watcher;
    std::map<int, int> helpers;
public:
    acceptor(ev::dynamic_loop& _loop, const std::string& ip, const int& port);
    void append_helper(int fd);
    void helper_died(int fd);
    void new_connection(ev::io& watcher, int);
    void read_from_helper(ev::io& watcher, int);
    ~acceptor();
};


#endif //HTTPSERVER_ACCEPTOR_H
