//
// Created by IsThisLoss on 24.10.16.
//

#include <iostream>
#include "CrashHandler.h"

bool CrashHandler::cont = true;
pid_t CrashHandler::acceptor_pid = 0;
int CrashHandler::acceptor_fd = -1;
std::list<pid_t> CrashHandler::children = {};
Acceptor* CrashHandler::accpt = nullptr;
Helper* CrashHandler::hlpr = nullptr;

void CrashHandler::run(const std::string& ip, const int& port)
{
    /*
     * initializing handlers for SIGTERM and SIGCHIL in the main process
     */
    logs << "Starting..." << std::endl;
    struct sigaction act;
    act.sa_handler = &CrashHandler::terminate;
    sigaction(SIGTERM, &act, nullptr);

    struct sigaction act2;
    act2.sa_handler = &CrashHandler::child_died;
    act2.sa_flags = SA_SIGINFO;
    sigaction(SIGCHLD, &act2, nullptr);

    create_acceptor(ip, port);

    for (unsigned i(0); i < 4 && cont; i++)
        create_helper();

    /*
     * it is some sort of infinite loop, which wait for signals
     * but it also break when crash handler get SIGTERM
     */
    while (cont)
        pause();
}

void CrashHandler::create_helper()
{
    int fd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
        throw std::runtime_error("Cannot create socketpair");

    switch (pid_t pid = fork())
    {
        case -1:
            throw std::runtime_error("Bad fork, errno = " + errno);
        case 0:
        {
            /* Beginning of child process, which work which connected clients*/
            ev::default_loop loop;

            hlpr = new Helper(fd[0]);
            struct sigaction act;
            act.sa_handler= &CrashHandler::helper_terminate;
            sigaction(SIGTERM, &act, nullptr);

            logs << "Helper was created, pid = [" << getpid() << ']' << std::endl;
            logs.flush();

            loop.run(0);
            cont = false;
            /* End of that process */
        }
        default:
        {
            char msg[] = "New Helper";
            sock_fd_write(acceptor_fd, msg, sizeof(msg), fd[1]);
            children.push_back(pid);
            close(fd[0]);
            close(fd[1]);
        }
    }
}

void CrashHandler::create_acceptor(const std::string& ip, const int& port)
{
    int fd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
        throw std::runtime_error("Cannot create socket pair, errno = " + errno);

    switch (acceptor_pid = fork())
    {
        case -1:
            throw std::runtime_error("Bad fork, errno = " + errno);
        case 0:
        {
            ev::default_loop loop;

            accpt = new Acceptor(fd[0], ip, port);
            struct sigaction act;
            act.sa_handler = &CrashHandler::acceptor_terminate;
            sigaction(SIGTERM, &act, nullptr);

            logs << "Acceptor was created, pid = [" << getpid() << ']' << std::endl;

            loop.run(0);
            cont = false;
        }
        default:
        {
            acceptor_fd = fd[1];
            //close(fd[0]);
        }
    }
}

void CrashHandler::acceptor_terminate(int)
{
    logs << "Acceptor [" << getpid() << "] has terminated" << std::endl;
    delete accpt;
}

void CrashHandler::helper_terminate(int)
{
    logs << "Helper [" << getpid() << "] has terminated" << std::endl;
    logs.close();
    delete hlpr;
}

void CrashHandler::terminate(int)
{
    for (auto& child : children)
        kill(child, SIGTERM);

    kill(acceptor_pid, SIGTERM);
    cont = false;
    logs.close();
}

void CrashHandler::child_died(int)
{
    int e;
    pid_t pid = wait(&e);
    logs << '[' << pid << "] died with exit code = " << e << std::endl;
    if (pid == acceptor_pid)
        raise(SIGTERM);
    else
    {
        children.erase(std::find(children.begin(), children.end(), pid));
        if (e != 0)
            create_helper();
    }
}
