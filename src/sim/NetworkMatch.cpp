#include "NetworkMatch.hpp"

NetworkMatch::NetworkMatch(const win::Area<float> &area)
	: area(area)
{
	reset();
}

void NetworkMatch::reset()
{
	server = win::UdpServer();
	client = win::UdpClient();
	guestid.reset();
	state = MatchState::disconnected;
	error = ErrorReason::none;
	client_sent = false;
	last_receive_time = std::chrono::steady_clock::now();

	host_counter = 0;
	guest_counter = 0;

	if (botsim)
		botsim.reset();
}

void NetworkMatch::start_bot()
{
    if (state != MatchState::disconnected && state != MatchState::listening)
    	win::bug("Trying to start bot but in state " + std::to_string((int)state) + " instead");

	botsim.reset(new Simulation(area, true));
	state = MatchState::listening;
}

bool NetworkMatch::host()
{
	if (state != MatchState::disconnected && state != MatchState::listening)
		win::bug("Trying to listen but in state " + std::to_string((int)state) + " instead");

	state = MatchState::listening;

	if (!server)
	{
		server = win::UdpServer(PORT);
		if (!server)
		{
			error = ErrorReason::cant_listen;
			state = MatchState::errored;
			return false;
		}
	}

	// fake the last receive time
	last_receive_time = std::chrono::steady_clock::now();

	float dummy;
	if (host_get_data(dummy))
	{
		state = MatchState::hosting;
		return true;
	}

	return false;
}

bool NetworkMatch::join(const char *ip)
{
	if (state != MatchState::disconnected && state != MatchState::joining)
		win::bug("Trying to join but in state " + std::to_string((int)state) + " instead");

	state = MatchState::joining;

	if (!client)
	{
		client = win::UdpClient(ip, PORT);

		// fake the last receive time
		last_receive_time = std::chrono::steady_clock::now();
	}

	if (!client)
	{
		state = MatchState::errored;
		error = ErrorReason::bad_address;
		return false;
	}

	float dummy = 0.0f;
	guest_send_data(dummy);

	float dummy1, dummy2, dummy3;
	int dummy4, dummy5;
	if (guest_get_data(dummy1, dummy2, dummy3, dummy4, dummy5))
	{
		state = MatchState::joined;
		return true;
	}

	return false;
}

bool NetworkMatch::hosting() const
{
	return state == MatchState::hosting;
}

NetworkMatch::ErrorReason NetworkMatch::errored() const
{
	return state == MatchState::errored ? error : ErrorReason::none;
}

bool NetworkMatch::host_get_data(float &guest_paddle_y)
{
	if (state != MatchState::hosting && state != MatchState::listening)
		win::bug("Trying to host but in state " + std::to_string((int)state) + " instead");

	if (!server)
	{
		state = MatchState::errored;
		error = ErrorReason::unknown;
		return false;
	}

	if (std::chrono::duration<float>(std::chrono::steady_clock::now() - last_receive_time).count() > DISCONNECT_SECONDS)
	{
		state = MatchState::errored;
		error = ErrorReason::lost_connection;
		return false;
	}

	unsigned char payload[8];
	unsigned char receiving[8];
	bool received = false;
	while (server.recv(receiving, sizeof(receiving), guestid) == sizeof(receiving))
	{
		int counter;
		memcpy(&counter, receiving, sizeof(counter));
		if (counter > guest_counter)
		{
			guest_counter = counter;
			received = true;
			memcpy(payload, receiving, sizeof(payload));
		}
	}

	if (!received)
		return false;

	memcpy(&guest_paddle_y, payload + 4, sizeof(guest_paddle_y));
	last_receive_time = std::chrono::steady_clock::now();

	return true;
}

void NetworkMatch::host_send_data(float host_paddle_y, float ball_x, float ball_y, int host_score, int guest_score)
{
	if (state != MatchState::hosting && state != MatchState::listening)
		win::bug("Trying to host but in state " + std::to_string((int)state) + " instead");

	if (!guestid.initialized)
		return; // don't know who to send it to yet

	if (!server)
	{
		state = MatchState::errored;
		error = ErrorReason::unknown;
		return;
	}

	unsigned char payload[18];
	unsigned char c;

	++host_counter;
	memcpy(payload + 0, &host_counter, sizeof(host_counter));
	memcpy(payload + 4, &host_paddle_y, sizeof(host_paddle_y));
	memcpy(payload + 8, &ball_x, sizeof(host_paddle_y));
	memcpy(payload + 12, &ball_y, sizeof(host_paddle_y));
	c = host_score;
	memcpy(payload + 16, &c, sizeof(c));
	c = guest_score;
	memcpy(payload + 17, &c, sizeof(c));

	server.send(payload, sizeof(payload), guestid);
}

bool NetworkMatch::guest_get_data(float &host_paddle_y, float &ball_x, float &ball_y, int &host_score, int &guest_score)
{
	if (state != MatchState::joined && state != MatchState::joining)
		win::bug("Trying to be joined but in state " + std::to_string((int)state) + " instead");

	if (!client)
	{
		state = MatchState::errored;
		error = ErrorReason::unknown;
		return false;
	}

	if (!client_sent)
		return false;

	if (std::chrono::duration<float>(std::chrono::steady_clock::now() - last_receive_time).count() > DISCONNECT_SECONDS)
	{
		state = MatchState::errored;
		error = ErrorReason::lost_connection;
		return false;
	}

	unsigned char payload[18];
	unsigned char receiving[18];

	bool received = false;

	while (client.recv(receiving, sizeof(receiving)) == sizeof(receiving))
	{
		int counter;
		memcpy(&counter, receiving, sizeof(counter));
		if (counter > host_counter)
		{
			host_counter = counter;
			received = true;
			memcpy(payload, receiving, sizeof(payload));
		}
	}

	if (!received)
		return false;

	float f;
	unsigned char c;

	memcpy(&f, payload + 4, sizeof(f));
	host_paddle_y = f;

	memcpy(&f, payload + 8, sizeof(f));
	ball_x = f;

	memcpy(&f, payload + 12, sizeof(f));
	ball_y = f;

	memcpy(&c, payload + 16, sizeof(c));
	host_score = c;

	memcpy(&c, payload + 17, sizeof(c));
	guest_score = c;

	last_receive_time = std::chrono::steady_clock::now();

	return true;
}

void NetworkMatch::guest_send_data(float guest_paddle_y)
{
	if (state != MatchState::joined && state != MatchState::joining)
		win::bug("Trying to be joined but in state " + std::to_string((int)state) + " instead");

	if (!client)
	{
		state = MatchState::errored;
		error = ErrorReason::unknown;
		return;
	}

	unsigned char payload[8];

	++guest_counter;
	memcpy(payload + 0, &guest_counter, sizeof(guest_counter));
	memcpy(payload + 4, &guest_paddle_y, sizeof(guest_paddle_y));

	client.send(payload, sizeof(payload));
	client_sent = true;
}
