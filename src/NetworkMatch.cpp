#include "NetworkMatch.hpp"

NetworkMatch::NetworkMatch(bool host, const std::string &connect_to_ip)
	: client_sent(false)
{
	if (host)
		server = win::UdpServer(PORT);
	else
		client = win::UdpClient(connect_to_ip, PORT);
}

bool NetworkMatch::host_get_data(float &guest_paddle_y)
{
	if (!server)
		win::bug("Host: can't use server socket!");

	float receiving;
	bool received = false;
	while (server.recv(&receiving, 4, guestid) == sizeof(receiving))
	{
		received = true;
		guest_paddle_y = receiving;
	}

	if (!received)
		return false;

	return true;
}

void NetworkMatch::host_send_data(float host_paddle_y, float ball_x, float ball_y, int host_score, int guest_score)
{
	if (!guestid.initialized)
		return; // don't know who to send it to yet

	if (!server)
		win::bug("Host: can't use server socket!");

	unsigned char payload[14];
	unsigned char c;

	memcpy(payload + 0, &host_paddle_y, sizeof(host_paddle_y));
	memcpy(payload + 4, &ball_x, sizeof(host_paddle_y));
	memcpy(payload + 8, &ball_y, sizeof(host_paddle_y));
	c = host_score;
	memcpy(payload + 12, &c, sizeof(c));
	c = guest_score;
	memcpy(payload + 13, &c, sizeof(c));

	server.send(payload, sizeof(payload), guestid);
}

bool NetworkMatch::guest_get_data(float &host_paddle_y, float &ball_x, float &ball_y, int &host_score, int &guest_score)
{
	if (!client)
		win::bug("Guest: can't use client socket!");

	if (!client_sent)
		return false;

	unsigned char payload[14];
	unsigned char receiving[14];

	bool received = false;

	while (client.recv(receiving, sizeof(receiving)) == sizeof(receiving))
	{
		received = true;
		memcpy(payload, receiving, sizeof(payload));
	}

	if (!received)
		return false;

	float f;
	unsigned char c;

	memcpy(&f, payload + 0, sizeof(f));
	host_paddle_y = f;

	memcpy(&f, payload + 4, sizeof(f));
	ball_x = f;

	memcpy(&f, payload + 8, sizeof(f));
	ball_y = f;

	memcpy(&c, payload + 12, sizeof(c));
	host_score = c;

	memcpy(&c, payload + 13, sizeof(c));
	guest_score = c;

	return true;
}

void NetworkMatch::guest_send_data(float guest_paddle_y)
{
	if (!client)
		win::bug("Guest: can't use client socket!");

	client.send(&guest_paddle_y, sizeof(guest_paddle_y));
	client_sent = true;
}
