//
// Created by IsThisLoss on 24.10.16.
//

#ifndef HTTPSERVER_CRASH_HANDLER_H
#define HTTPSERVER_CRASH_HANDLER_H

#include <list>
#include <string>
#include <stdexcept>

#include <sys/socket.h>
#include <unistd.h>
#include <wait.h>

#include <ev++.h>

#include "util.h"
#include "Acceptor.h"
#include "Helper.h"

/**
 * @class CrashHandler
 *
 * @brief creates acceptor and helpers
 *
 * Is is the main class, which create and run
 * one process to accept new connections
 * and predefined number of process to work with already
 * connected clients
 */
class CrashHandler final
{
private:

    /**
     * @brief the variable which true only for crash handler process
     * for other process it is always false
     */
    static bool cont;

    /**
     * @brief pid of process which listen for new connections
     */
    static pid_t acceptor_pid;

    /**
     * @brief socket of process which listen for new connections
     */
    static int acceptor_fd;

    /**
     * @brief list of pids of child process
     */
    static std::list<pid_t> children;

    /**
     * @brief creates new process to listen new connections
     * @param ip address for listen
     * @param port which port will be listened
     */
    static void create_acceptor(const std::string& ip, const int& port);

    /**
     * @brief creates new process to work with already connected clients
     */
    static void create_helper();

    /* signals handlers */

    /**
     * @brief handler for SIGTERM, which was sent to crash handler process
     */
    static void terminate(int);

    /**
     * @brief handler for SIGCHLD of crash handler process
     */
    static void child_died(int);

    /* Acceptor section - this section only for use into Acceptor process*/

    /**
     * @brief handler for SIGTERM of acceptor process
     */
    static void acceptor_terminate(int);

    /**
     * @brief pointer to instance of acceptor class
     */
    static Acceptor* accpt;

    /* Helper section - this section only for use into Helper process*/

    /**
     * @brief pointer to instance of helper class (only one)
     */
    static Helper* hlpr;

    /**
     * handler for SIGTERM of helper process
     */
    static void helper_terminate(int);
public:

    /**
     * @brief this function start crash handler, acceptor process and creates helpers
     * it just starts all what is needed
     * @param ip ip address which server will be listened
     * @param port port of that address which server will be listened
     */
    static void run(const std::string& ip, const int& port);
};


#endif //HTTPSERVER_CRASH_HANDLER_H
