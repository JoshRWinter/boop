#include "NetworkMatch.hpp"

NetworkMatch::NetworkMatch(const win::Area<float> &area)
	: botsimexchanger(60)
	, area(area)
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

void NetworkMatch::start_bot(DifficultyLevel bot_difficulty)
{
    if (state != MatchState::disconnected && state != MatchState::listening)
    	win::bug("Trying to start bot but in state " + std::to_string((int)state) + " instead");

	botsim.reset(new Simulation(NULL, area, true, bot_difficulty, botsimexchanger));
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

	int dummy;
	float dummy2;
	if (host_get_data(dummy, dummy2))
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

	guest_send_data(0, 0.0f);

	int dummy1;
	float dummy2;
	if (guest_get_data(dummy1, dummy2, dummy2, dummy2, dummy2, dummy2, dummy2, dummy1, dummy1))
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

bool NetworkMatch::host_get_data(int &guest_paddle_color, float &guest_paddle_y)
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

	unsigned char payload[9];
	unsigned char receiving[9];
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

	unsigned char char_guest_paddle_color;
	memcpy(&guest_paddle_y, payload + 4, sizeof(guest_paddle_y));
	memcpy(&char_guest_paddle_color, payload + 8, sizeof(char_guest_paddle_color));
	last_receive_time = std::chrono::steady_clock::now();

	guest_paddle_color = char_guest_paddle_color;

	return true;
}

void NetworkMatch::host_send_data(int host_paddle_color, float host_paddle_y, float paddle_height, float ball_x, float ball_y, float ball_xv, float ball_yv, int host_score, int guest_score)
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

	unsigned char payload[31];
	unsigned char c;

	++host_counter;
	memcpy(payload + 0, &host_counter, sizeof(host_counter));
	memcpy(payload + 4, &host_paddle_y, sizeof(host_paddle_y));
	memcpy(payload + 8, &paddle_height, sizeof(paddle_height));
	memcpy(payload + 12, &ball_x, sizeof(ball_x));
	memcpy(payload + 16, &ball_y, sizeof(ball_x));
	memcpy(payload + 20, &ball_xv, sizeof(ball_xv));
	memcpy(payload + 24, &ball_yv, sizeof(ball_yv));
	c = host_score;
	memcpy(payload + 28, &c, sizeof(c));
	c = guest_score;
	memcpy(payload + 29, &c, sizeof(c));
	c = host_paddle_color;
	memcpy(payload + 30, &c, sizeof(c));

	server.send(payload, sizeof(payload), guestid);
}

bool NetworkMatch::guest_get_data(int &host_paddle_color, float &host_paddle_y, float &paddle_height, float &ball_x, float &ball_y, float &ball_xv, float &ball_yv, int &host_score, int &guest_score)
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

	unsigned char payload[31];
	unsigned char receiving[31];

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
	paddle_height = f;

	memcpy(&f, payload + 12, sizeof(f));
	ball_x = f;

	memcpy(&f, payload + 16, sizeof(f));
	ball_y = f;

	memcpy(&f, payload + 20, sizeof(f));
	ball_xv = f;

	memcpy(&f, payload + 24, sizeof(f));
	ball_yv = f;

	memcpy(&c, payload + 28, sizeof(c));
	host_score = c;

	memcpy(&c, payload + 29, sizeof(c));
	guest_score = c;

	memcpy(&c, payload + 30, sizeof(c));
	host_paddle_color = c;

	last_receive_time = std::chrono::steady_clock::now();

	return true;
}

void NetworkMatch::guest_send_data(int guest_paddle_color, float guest_paddle_y)
{
	if (state != MatchState::joined && state != MatchState::joining)
		win::bug("Trying to be joined but in state " + std::to_string((int)state) + " instead");

	if (!client)
	{
		state = MatchState::errored;
		error = ErrorReason::unknown;
		return;
	}

	const unsigned char char_guest_paddle_color = guest_paddle_color;

	unsigned char payload[9];

	++guest_counter;
	memcpy(payload + 0, &guest_counter, sizeof(guest_counter));
	memcpy(payload + 4, &guest_paddle_y, sizeof(guest_paddle_y));
	memcpy(payload + 8, &char_guest_paddle_color, sizeof(char_guest_paddle_color));

	client.send(payload, sizeof(payload));
	client_sent = true;
}
