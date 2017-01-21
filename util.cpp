#include <stdexcept>
#include "util.h"

ssize_t sock_fd_write(int sock, void* buff, ssize_t bufflen, int fd)
{
    ssize_t size;
    struct msghdr msg;
    struct iovec iov;
    union
    {
        struct cmsghdr cmsghdr;
        char control[CMSG_SPACE(sizeof(int))];
    } cmsgu;
    struct cmsghdr* cmsg;
    iov.iov_base = buff;
    iov.iov_len = bufflen;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    if (fd != -1)
    {
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        *((int*)CMSG_DATA(cmsg)) = fd;
    }
    else
    {
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
    }
    size = sendmsg(sock, &msg, 0);
    if (size < 0)
        throw std::runtime_error("Passing fd: Error: write " + errno);
    return size;
}

ssize_t sock_fd_read(int sock, void* buff, ssize_t bufflen, int* fd)
{
    ssize_t size;
    if(fd)
    {
        struct msghdr msg;
        struct iovec iov;
        union
        {
            struct cmsghdr cmsghdr;
            char constrol[CMSG_SPACE(sizeof(int))];
        } cmsgu;
        struct cmsghdr* cmsg;
        iov.iov_base = buff;
        iov.iov_len = bufflen;
        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = cmsgu.constrol;
        msg.msg_controllen = sizeof(cmsgu.constrol);
        size = recvmsg(sock, &msg, 0);
        if (size < 0)
            throw std::runtime_error("Passing fd: Error: read");
        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(int)))
        {
            if (cmsg->cmsg_level != SOL_SOCKET)
                throw std::runtime_error("Passing fd: Error: read: invalid cmsg_level");
            if (cmsg->cmsg_type != SCM_RIGHTS)
                throw std::runtime_error("Passing fd: Error: read: invalid cmsg_type");
            *fd = *((int *) CMSG_DATA(cmsg));
        }
        else
            *fd = -1;
    }
    else
    {
        size = read(sock, buff, bufflen);
        if (size < 0)
            throw std::runtime_error("Passing fd: Error: read");
    }
    return size;
}

int set_nonblock(int fd)
{
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}
