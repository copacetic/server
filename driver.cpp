#include "Login.hpp"

int main(int argc, char* argv[])
{
    bool debug = false;
    if(argc != 1)
        debug = true;
    Login game(debug);
	game.setup_server();
	game.setup_mq();
	game.start_epoll_thread();
	game.accept_connections();
	return 0;
}

