#include "EpollManager.hpp"

EpollManager::EpollManager(){}

EpollManager::EpollManager(bool _debug)
{
    debug = _debug;
	epfd = 0;
	numConnected = 0;
	events = new epoll_event[MAX_EVENTS];
    ev.events = EPOLLIN;
	cout << "Successfully created events array...\n";
	create_epoll(30); // expect 30 connections
	add_to_set(0); // add stdin
	setup_mq();
	add_to_set(mq); // add mq to the set...
    disc_comm[0] = 2;
    disc_comm[1] = 25;
    reg_comm[0] = 2;
    reg_comm[1] = 21;
}

void EpollManager::notify_new_player_of_existing_ones(int newPlayer)
{
    s_send(newPlayer, reg_comm, 3, MSG_NOSIGNAL);
    for(iMap = connections.begin(); iMap != connections.end(); iMap++) // instead of iterators consider maintaining
    {                                                                  // an array of file descriptors. Very fast iteration.
        if(iMap->first != newPlayer)
        {
            reg_comm[2] = iMap->first;
            if(s_send(newPlayer, reg_comm, 3, MSG_NOSIGNAL))
            {
                if(debug)
                {
                    cout << "Sending to new player: ";
                    print_buffer(reg_comm, 3);
                }
            }
        }
    } // let the new player know of all the old players.. and themself
}

void EpollManager::print_buffer(unsigned char buf[], int bufLen)
{
    for(int i = 0; i < bufLen; i++)
        cout << (int)buf[i] << " ";
    cout << '\n';
}

void EpollManager::broadcast(unsigned char msg[], int bytesRead, int sender)
{
	for(iMap = connections.begin(); iMap != connections.end(); iMap++)
	{
		if(sender != iMap->first)
		{
			if(s_send(iMap->first, msg, bytesRead, MSG_NOSIGNAL))
			{
			    if(debug)
			    {
                    print_buffer(msg, bytesRead);
                    cout << "sent on file descriptor " << iMap->first << '\n';
			    }
			}
		}
	}
	if(connections.find(sender) != connections.end())
        connections[sender]->reset_batch();
}

void EpollManager::disconnect(int fd)
{
	// remove from epoll, release memory, remove from map, close socket
	// unimportant kernel flaw requires some event
	if(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev) ==  -1) perror("Epoll_ctl failed");
    delete connections[fd];
    connections.erase(fd);
    cout << "Removed from map: " << fd << "\n";

    if(close(fd) == -1)
        perror("Close failed");
	else
        cout << "Closed fd: " << fd << '\n';
    numConnected--;

    disc_comm[2] = fd;
    broadcast(disc_comm, 3, fd);
}

bool EpollManager::s_send(int curFD, unsigned char buf[], int bufLen, int flag) // Meant to counteract partial sends
{
    int sendRetVal = 0;
    int bytesSent = 0;
    while(bytesSent != bufLen)
    {
        sendRetVal = send(curFD, buf + bytesSent, bufLen - bytesSent, flag);
        if(sendRetVal == -1)
        {
            perror("Sending failed");
            return false;
        }
        else
            bytesSent += sendRetVal;
    }
    return true;
}

bool EpollManager::s_recv(int curFD, unsigned char buf[], int& bytesRead)
{
    bytesRead = recv(curFD, buf, BUF_LENGTH, 0);
	if(bytesRead == -1) // error
    {
        perror("Receiving failed");
        return false;
    }
    if(bytesRead == 0) // disconnect
    {
       disconnect(curFD);
       return false;
    }
    return true; // success
}

bool EpollManager::check_length(int length, int curFD)
{
    if(length > (BUF_LENGTH - 1) || length == 0)
    {
        disconnect(curFD);
        return false;
    }
    return true;
}

void EpollManager::handle_admin_input()
{
    unsigned char buf[3];
    cin >> buf; // maybe need to put a null byte somewhere
    switch(buf[0])
    {
        case 'q':
            mq_close(mq);
            mq_unlink("/mqq");
            exit(EXIT_SUCCESS);
            break;
        case 'c':
            if(mq_close(mq) == -1 || mq_unlink("/mqq") == -1)
                perror("Mq_close or unlink failed");
            else
                cout << "MQ was closed and unlinked.\n";
            break;
        case 'd':
            disconnect(atoi((char*)(buf + 1)));
            break;
        default:
            cout << "Invalid admin command.\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////EPOLL_CODE////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void EpollManager::create_epoll(int est)
{
	epfd = epoll_create(est);
	if (epfd == -1)
	{
		perror("Epoll_create failed");
		exit(EXIT_FAILURE);
	}
}

void EpollManager::add_to_set(int fd)
{
	ev.data.fd = fd;
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		perror("Epoll_ctl add failed");
		exit(EXIT_FAILURE);
	}
	cout << fd << " added to epoll set...\n";
}

void EpollManager::process_epoll()
{
	int pos, rem, length, numEvents, i, curFD, *curCounter, *curLength, bytesRead = 0;
	unsigned char buf[BUF_LENGTH], *curBuf;
    Client* curClient;
	while(true) // main event processing loop
	{
		numEvents = epoll_wait(epfd, events, MAX_EVENTS, -1); // grabs events
		for (i = 0; i < numEvents; i++)
		{

		    if(events[i].data.fd == 0) // checks to see if event is on stdin
		    {
                handle_admin_input();
                continue;
		    }

			if(events[i].data.fd == mq) // checks to see if event is on listener
			{
			    handle_new_client();
                continue;
			}

            curFD = events[i].data.fd; // an existing client sent us data

            if(!s_recv(curFD, buf, bytesRead)) continue; // checks for errors
                                                         // and disconnects

            rem = 0; // rem is the unprocessed data in buf
            pos = 0; // pos is our current position in buf
            length = 0; // length is the length of the packet
                        // given by the first byte of a packet
            curClient = connections[curFD]; // query once.
            if(curClient->get_b_first()) // fresh packet
            {
                length = (int)buf[pos];
                if(!check_length(length, curFD)) continue; // checks for overflow case
                rem = bytesRead;
                buf_loop(bytesRead, curFD, rem, pos, length, buf);
                continue;
            }

            curBuf = curClient->get_b_buf();
            curCounter = curClient->get_b_counter();
            curLength = curClient->get_b_counter();
            // cheated here: using rem to be the expected remaining data to complete a packet
            // not the data left in buf... consider changing later
            rem = (*curLength) - (*curCounter);

            if(bytesRead >= rem)
            {
                memcpy(curBuf + *curCounter, buf, rem);
                broadcast(curBuf, *curLength, curFD);
                pos = rem;
                if(pos != bytesRead)
                {
                    length = (int)buf[pos];
                    if(!check_length(length, curFD)) continue;
                }
                rem = bytesRead - rem;
                buf_loop(bytesRead, curFD, rem, pos, length, buf, curBuf, curClient);
                continue;
            }

            // bytesRead is less than the amount req'd to finish the packet
            memcpy(curBuf + (*curCounter), buf, bytesRead);
            curClient->inc_b_counter(bytesRead);
		}
	}
}

void EpollManager::buf_loop(int bytesRead, int curFD, int &rem, int &pos, int &length, unsigned char buf[])
{
    Client* curClient = connections[curFD];
    unsigned char* curBuf = curClient->get_b_buf();
	while(length < rem) // lengthOfPacket < bytes in read
	{
		memcpy(curBuf, buf + pos, length + 1); // dont do so many method calls
		broadcast(curBuf, length + 1, curFD);
		rem = rem - (length + 1);
		pos = pos + length + 1;
		if(pos != bytesRead)
		{
			length = (int)buf[pos];
			if(!check_length(length, curFD)) return;
        }
	}
    if(rem != 0)
    {
		memcpy(curBuf + *(curClient->get_b_counter()), buf + pos, rem);
		curClient->set_b_length(length + 1);
		curClient->inc_b_counter(rem);
		curClient->set_b_first(false);
	}
}

void EpollManager::buf_loop(int bytesRead, int curFD, int &rem, int &pos, int &length, unsigned char buf[], unsigned char* curBuf, Client* curClient)
{
	while(length < rem) // lengthOfPacket < bytes in read
	{
		memcpy(curBuf, buf + pos, length + 1);
		broadcast(curBuf, length + 1, curFD);
		rem = rem - (length + 1);
		pos = pos + length + 1;
		if(pos != bytesRead)
		{
			length = (int)buf[pos];
			if(!check_length(length, curFD)) return;
        }
	}
    if(rem != 0)
    {
		memcpy(curBuf + *(curClient->get_b_counter()), buf + pos, rem);
		curClient->set_b_length(length + 1);
		curClient->inc_b_counter(rem);
		curClient->set_b_first(false);
	}
}

//////////////////////////////////MQ_CODE/////////////////////////////////////////
void EpollManager::setup_mq()
{
	char name[] = "/mqq";

	mq = mq_open(name, O_RDONLY);
	if(mq == -1)
	{
        perror("Mq_open failed");
        exit(EXIT_FAILURE);
	}
}

void EpollManager::handle_new_client()
{
    char byteArray[4];
    int numRecved = mq_receive(mq, byteArray, 4, NULL);
	if(numRecved == -1)
	{
	    perror("Mq_receive failed");
        exit(EXIT_FAILURE);
	}

	Client * temp = (Client*)(*((int*)byteArray));
	int id = temp->get_id();
	connections[id] = temp;
	add_to_set(id);

    // set up the login command
    reg_comm[2] = id;

    numConnected++;

    broadcast(reg_comm, 3, id); // let everyone know there's a new player
    notify_new_player_of_existing_ones(id);
}

void EpollManager::operator()()
{
    process_epoll();
}

