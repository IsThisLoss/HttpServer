#ifndef HTTPSERVER_SERVER_H
#define HTTPSERVER_SERVER_H


#include <map>

#include <ev++.h>

#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <wait.h>

#include "acceptor.h"
#include "helper.h"


class server final
{
private:
    /* some sort of singleton */
    server() {}
    ~server() {}
    /* pointers due to free memory */
    static acceptor* a;
    static helper* h;
    static std::map<pid_t, int> children;
    static bool am_i_acceptor;
public:
    static void run(const std::string& ip, const int& port);
    static void create_helper();
    static void terminate(int, siginfo_t*, void*);
    static void parent_recreate_helper(int, siginfo_t* info, void*);
};

#endif //HTTPSERVER_SERVER_H
