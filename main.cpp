#include <iostream>

#include <unistd.h>

#include "server.h"


int main(int argc, char** argv)
{
    int r, port = 0;
    std::string ip;
    std::string dir;
    while((r = getopt(argc, argv, "h:p:d:")) != -1)
           switch (r)
           {
               case 'h':
                   ip = optarg;
                   break;
               case 'p':
                   port = atoi(optarg);
                   break;
               case 'd':
                   dir = optarg;
                   break;
               default:
               {
                   std::cerr << "Wrong argument" << std::endl;
                   return 2;
               }
           }
    if (ip.empty() || port == 0 || dir.empty())
    {
        std::cerr << "Wrong argument" << std::endl;
        return 2;
    }



    daemon(1,0);
    chdir(dir.c_str());
    chroot(dir.c_str());
    try
    {
        server::run(ip, port);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}