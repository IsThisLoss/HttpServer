//#include <iostream>
#include "server.h"

acceptor* server::a = nullptr;
helper* server::h = nullptr;
std::map<pid_t, int> server::children = {};
bool server::am_i_acceptor = true;

void server::run(const std::string& ip, const int& port)
{
    struct sigaction term, parent_chld;
    term.sa_sigaction = &server::terminate;
    //sigaddset(&term.sa_mask, SIGCHLD);
    sigaction(SIGTERM, &term, NULL);

    parent_chld.sa_sigaction = &server::parent_recreate_helper;
    sigaction(SIGCHLD, &parent_chld, NULL);

    ev::dynamic_loop loop;
    a = new acceptor(loop, ip, port);
    server::am_i_acceptor = true;
    for (int i(0); i < 4 && am_i_acceptor; i++)
        server::create_helper();
    if(server::am_i_acceptor)
        loop.run(0);
}

void server::create_helper()
{
    int fd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
        throw std::runtime_error("Cannot create socket pair, errno = " + errno);
    pid_t child_pid;
    switch (child_pid = fork())
    {
        case -1:
            throw std::runtime_error("Bad fork, errno = " + errno);
        case 0:
        {
            server::am_i_acceptor = false;
            ev::dynamic_loop loop;
            //std::cout << getpid() << std::endl;
            server::h = new helper(loop, fd[0]);
            loop.run(0);
        }
        default:
        {
            server::children.insert(std::make_pair(child_pid, fd[1]));
            server::a->append_helper(fd[1]);
        }
    }
}

void server::terminate(int, siginfo_t*, void*)
{
    if (am_i_acceptor)
    {
        for (auto& i : server::children)
        {
            kill(i.first, SIGTERM);
            //std::cerr << i.first << std::endl;
        }
        delete server::a;
    }
    else
    {
        //std::cerr << "+\n";
        delete server::h;
    }
}

void server::parent_recreate_helper(int, siginfo_t* info, void*)
{
    int e;
    pid_t pid = wait(&e);
    if (e != 0)
    {
        server::a->helper_died(server::children.at(pid));
        server::children.erase(pid);
        server::create_helper();
    }
}