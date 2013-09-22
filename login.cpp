#include "Login.hpp"
#include "EpollManager.hpp"

Login::Login(){}

Login::Login(bool _debug)
{
    debug = _debug;

	char defPort[] = "3500";
	port = new char[strlen(defPort)];
	cout << "Setting port to 3500...\n";
	strncpy(port, defPort, strlen(defPort));

	cout << "Setting server details...\n";
	memset(&details, 0, sizeof(details));
	details.ai_family = AF_INET;
	details.ai_socktype = SOCK_STREAM;
	details.ai_flags = AI_PASSIVE;
}

void Login::setup_server()
{
	getaddrinfo(NULL, port, &details, &addrList);
	for(addrIter = addrList; addrIter != NULL; addrIter = addrIter->ai_next)
	{
		if(!create_socket())
			continue;
		if(!set_reusable())
			exit(1);
		if(!bind_socket())
			continue;
        listen_on_socket();
		cout << "Server setup complete...\n";
		break;
	}

	if(addrIter == NULL)
	{
		perror("Failed to bind");
		return;
	}
	freeaddrinfo(addrList);
}

bool Login::create_socket()
{
	if((mainFd = socket(addrIter->ai_family, addrIter->ai_socktype, addrIter->ai_protocol)) == -1)
	{
		perror("Socket creation failed.");
		return false;
	}
	cout << "Socket creation succeeded...\n";
	return true;
}

bool Login::set_reusable()
{
	int yes = 1;
	if (setsockopt(mainFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
        perror("Setting as reusable failed");
        return false;
    }
	cout << "Setting as reusable succeeded...\n";
	return true;
}

bool Login::bind_socket()
{
	if (bind(mainFd, addrIter->ai_addr, addrIter->ai_addrlen) == -1)
	{
		perror("Binding failed.");
		close(mainFd);
		return false;
	}
	cout << "Binding succeeded...\n";
	return true;
}

void Login::listen_on_socket()
{
	if(listen(mainFd, BACKLOG) == -1)
	{
		perror("Listening failed");
		exit(1);
	}
	cout << "Listening succeeded...\n";
}

void Login::accept_connections() // needs to be some type of while loop
{
    int newPlayer;
    socklen_t addr_size = sizeof(client_addr);
    while(true)
    {
        newPlayer = accept(mainFd, (sockaddr *)&client_addr, &addr_size);
        if(newPlayer == -1)
        {
            perror("Accept failed");
        }
        else
        {
            cout << "Connection accepted.\n";
            Client* temp = new Client;
            temp->set_id(newPlayer); // id is also the fd
            // translate temp pointer to array of bytes and send over mq here
            char* byteArray = (char*)&temp;
            int numSent = mq_send(mq, byteArray, 4, 0);
            if(numSent == -1)
            {
                perror("Mq_send failed");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void Login::setup_mq()
{
	mq_attr attr;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 4;
	attr.mq_flags = 0;
	attr.mq_curmsgs = 0;

	char name[] = "/mqq";

	mq = mq_open(name, O_CREAT|O_WRONLY, S_IRUSR, &attr);
	if(mq == -1)
	{
        perror("Mq_open failed");
        exit(EXIT_FAILURE);
	}
}

void Login::start_epoll_thread()
{
    EpollManager man(debug);
    boost::thread workerThread(man);
    cout << "Epoll thread started.\n";
}
