#ifndef _EPOLLMANAGER
#define _EPOLLMANAGER

#define BUF_LENGTH 100
#define MAX_EVENTS 100

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include <netinet/tcp.h>
#include <netdb.h>
#include <netinet/in.h>

#include <mqueue.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <boost/unordered_map.hpp>
#include "Client.hpp"


using std::cout;
using std::cin;

class EpollManager
{
public:
	EpollManager();
	EpollManager(bool);
	void operator ()();
private:
	// epoll
    void process_epoll();
	void create_epoll(int);
	void add_to_set(int);
	void setup_mq();
    // active methods
	bool s_send(int, unsigned char[], int, int);
    bool s_recv(int, unsigned char[], int&);
	void disconnect(int);
    void notify_new_player_of_existing_ones(int);
    void print_buffer(unsigned char [], int);
    bool check_length(int, int);
	void handle_admin_input();
	void broadcast(unsigned char [], int, int);
    void buf_loop(int, int, int &, int &, int &, unsigned char []);
	void buf_loop(int, int, int &, int &, int &, unsigned char[], unsigned char*, Client*);
	void handle_new_client();
	// boost
	boost::unordered_map<int, Client*> connections;
	boost::unordered_map<int, Client*>::iterator iMap;

    unsigned char disc_comm[3]; // disc command buffer
    unsigned char reg_comm[3]; // register command buffer
	bool debug;
	// Linux API variables and primitives
	int epfd, mq, numConnected;
    epoll_event ev;
    char * port;
	epoll_event * events;
};

#endif
