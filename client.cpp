//#include "Client.hpp"

Client::Client()
{
	reset_batch();
}

//stat methods
void Client::set_location(float _x, float _y, float _z, float _r)
{
	x = _x;
	y = _y;
	z = _z;
	r = _r;
}

void Client::set_name(char _name[])
{
	int lenName = strlen(_name);
	name = new char[lenName];
	strncpy(name, _name, lenName);
}

void Client::reset_batch()
{
	b.firstByte = true;
	b.counter = 0;
	b.length = 0;
}
