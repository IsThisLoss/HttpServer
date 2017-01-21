#include "Helper.h"

Helper::Helper(int fd)
{
    acceptor_watcher = new ev::io;
    acceptor_watcher->set(fd, ev::READ);
    acceptor_watcher->set<Helper, &Helper::read_from_acceptor>(this);
    acceptor_watcher->start();
}

Helper::~Helper()
{
    for (auto&& i : watchers)
    {
        i->stop();
        shutdown(i->fd, SHUT_RDWR);
        close(i->fd);
        delete i;
    }
    acceptor_watcher->stop();
    shutdown(acceptor_watcher->fd, SHUT_RDWR);
    close(acceptor_watcher->fd);
    delete acceptor_watcher;
}

void Helper::read_from_client(ev::io& watcher, int)
{
    char buff[2048];
    ssize_t size = recv(watcher.fd, buff, 2048, MSG_NOSIGNAL);
    if (size < 0)
        return;
    else if (size == 0) // connection was closed
    {
        watcher.stop();
        shutdown(watcher.fd, SHUT_RDWR);
        close(watcher.fd);
        auto it = std::find(watchers.begin(), watchers.end(), &watcher);
        if (it != watchers.end())
            watchers.erase(it);
        ev::io* w = &watcher;
        delete w;
        char discon[] = "disconnection";
        sock_fd_write(acceptor_watcher->fd, (void*)discon, sizeof(discon), -1);
    }
    else
    {
        std::string Response = resp.get_response(std::string(buff));
        send(watcher.fd, Response.c_str(), Response.length(), MSG_NOSIGNAL);
    }
}

void Helper::read_from_acceptor(ev::io& watcher, int)
{
    int fd;
    char buff[32];
    sock_fd_read(watcher.fd, (void*)buff, 32, &fd);
    if (fd > 0)
    {
        ev::io* client_watcher = new ev::io;
        client_watcher->set(fd, ev::READ);
        client_watcher->set<Helper, &Helper::read_from_client>(this);
        watchers.push_back(client_watcher);
        client_watcher->start();
    }
}



