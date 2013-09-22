#ifndef _LOGIN
#define _LOGIN

#include <boost/thread.hpp>

#include <iostream>
#include <fcntl.h>

#include <mqueue.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "Client.hpp"

#define BUF_LENGTH 100
#define MAX_EVENTS 100
#define BACKLOG 15

using std::cout;

class Login
{
public:
    Login();
    Login(bool);
    void setup_server();
    void setup_mq();
    void start_epoll_thread();
    void accept_connections();
    inline void set_debug(bool _debug){debug = _debug;};
private:
    bool create_socket();
    bool set_reusable();
    bool bind_socket();
    void listen_on_socket();

    bool debug;
    int mainFd, mq;
    char userPass[100];
    char * port;
	addrinfo details, *addrList, *addrIter;
	sockaddr_storage client_addr;
};
// TODO: Login thread will do SQL verification
// TODO: Will make a utilities class to do basics like send, receive
#endif
