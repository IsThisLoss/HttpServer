#include <iostream>
#include "Acceptor.h"

Acceptor::Acceptor(int fd, const std::string& ip, const int& port)
{
    int acceptor_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (acceptor_socket == -1)
        throw std::runtime_error("Cannot create accept socket, errno = " + errno);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (!inet_aton(ip.c_str(), &addr.sin_addr))
        throw "Wrong ip";
    addr.sin_port = htons(port);
    if (bind(acceptor_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("Bind error, errno = " + errno);
    if (listen(acceptor_socket, SOMAXCONN) == -1)
        throw std::runtime_error("Listen error, errno = " + errno);

    accept_watcher = new ev::io;
    accept_watcher->set(acceptor_socket, ev::READ);
    accept_watcher->set<Acceptor, &Acceptor::new_connection>(this);
    accept_watcher->start();

    crash_watcher = new ev::io;
    crash_watcher->set(fd, ev::READ);
    crash_watcher->set<Acceptor, &Acceptor::new_helper>(this);
    crash_watcher->start();
}

void Acceptor::new_connection(ev::io& watcher, int)
{
    int new_client = accept(watcher.fd, NULL, 0);
    if (new_client > 0)
    {
        logs << "New client" << std::endl;
        auto it = std::min_element(helpers.begin(), helpers.end(),
        [](std::pair<int, int> a, std::pair<int, int> b)
        {
            return a.second < b.second;
        });
        sock_fd_write(it->first, (void*)"1", 2, new_client);
        it->second++;
        close(new_client);
    }
}

void Acceptor::read_from_helper(ev::io& watcher, int)
{
    char buff[32];
    auto s = sock_fd_read(watcher.fd, (void*)buff, 32, NULL);
    if (s == 0)
    {
        watcher.stop();
        //shutdown(watcher.fd, SHUT_RDWR);
        close(watcher.fd);
        // let it will be just like this
        for (auto i = watchers.begin(); i != watchers.end(); i++)
            if ((*i)->fd == watcher.fd)
            {
                delete (*i);
                watchers.erase(i);
                break;
            }
    }
    if (strcmp(buff, "disconnection"))
        helpers[watcher.fd]--;

}

Acceptor::~Acceptor()
{
    for (auto i = watchers.begin(); i != watchers.end(); i++)
    {
        (*i)->stop();
        delete (*i);
    }

    for (auto& i : helpers)
    {
        shutdown(i.first, SHUT_RDWR);
        close(i.first);
    }
    accept_watcher->stop();
    close(accept_watcher->fd);
    delete accept_watcher;

    crash_watcher->stop();
    close(crash_watcher->fd);
    delete crash_watcher;
}

void Acceptor::new_helper(ev::io& watcher, int)
{
    char buff[32];
    int helper_fd;

    sock_fd_read(watcher.fd, (void*)buff, 32, &helper_fd);

    ev::io* helper_watcher = new ev::io;
    helper_watcher->set(helper_fd, ev::READ);
    helper_watcher->set<Acceptor, &Acceptor::read_from_helper>(this);
    helper_watcher->start();

    watchers.push_back(helper_watcher);
    helpers.insert(std::make_pair(helper_fd, 0));
}



