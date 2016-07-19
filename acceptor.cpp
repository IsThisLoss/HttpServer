#include "acceptor.h"

acceptor::acceptor(ev::dynamic_loop& _loop, const std::string& ip, const int& port) : loop(_loop)
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
    accept_watcher = new ev::io(loop);
    accept_watcher->set(acceptor_socket, ev::READ);
    accept_watcher->set<acceptor, &acceptor::new_connection>(this);
    accept_watcher->start();
}

void acceptor::new_connection(ev::io& watcher, int)
{
    int new_client = accept(watcher.fd, NULL, 0);
    if (new_client > 0)
    {
        auto it = std::min_element(helpers.begin(), helpers.end(),
        [](std::pair<int, int> a, std::pair<int, int> b)
        {
            return a.second < b.second;
        });
        sock_fd_write(it->first, (void*)"1", 2, new_client);
        it->second++;
    }
}

void acceptor::append_helper(int fd)
{
    ev::io* helper_watcher = new ev::io(loop);
    helper_watcher->set(fd, ev::READ);
    helper_watcher->set<acceptor, &acceptor::read_from_helper>(this);
    helpers.insert(std::make_pair(fd, 0));
    watchers.push_back(helper_watcher);
    helper_watcher->start();
}

void acceptor::read_from_helper(ev::io& watcher, int)
{
    char buff[32];
    sock_fd_read(watcher.fd, (void*)buff, 32, NULL);
    if (strcmp(buff, "disconnection"))
        watchers[watcher.fd]--;
}

acceptor::~acceptor()
{
    for (auto&& i : watchers)
    {
        i->stop();
        delete i;
    }
    for (auto& i : helpers)
    {
        shutdown(i.first, SHUT_RDWR);
        close(i.first);
    }
    loop.break_loop(ev::ALL);
    exit(0);
}

void acceptor::helper_died(int fd)
{
    for (auto i = watchers.begin(); i != watchers.end(); i++)
    {
        if((*i)->fd == fd)
            {
                (*i)->stop();
                watchers.erase(i);
                break;
            }
    }

    helpers.erase(fd);
    shutdown(fd, SHUT_RDWR);
    close(fd);
}



