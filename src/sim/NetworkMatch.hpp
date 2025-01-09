#pragma once

#include <string>
#include <memory>

#include <win/Win.hpp>
#include <win/Utility.hpp>
#include <win/UdpServer.hpp>
#include <win/UdpClient.hpp>

#include "Simulation.hpp"

class NetworkMatch
{
	WIN_NO_COPY_MOVE(NetworkMatch);

	enum class MatchState
	{
		disconnected,
		listening,
		hosting,
		joining,
		joined
	};

public:
	static constexpr int PORT = 28857;

	NetworkMatch(const win::Area<float> &area);

	void reset();
	void start_bot();
	bool host();
	bool join(const char *ip);
	bool hosting() const;

	// called by host
	bool host_get_data(float &guest_paddle_y);
	void host_send_data(float host_paddle_y, float ball_x, float ball_y, int host_score, int guest_score);

	// called by guest
	bool guest_get_data(float &host_paddle_y, float &ball_x, float &ball_y, int &host_score, int &guest_score);
	void guest_send_data(float guest_paddle_y);

private:
	win::Area<float> area;
	std::unique_ptr<Simulation> botsim;

	win::UdpServer server;
	win::UdpClient client;
	win::UdpId guestid;

	int host_counter;
	int guest_counter;

	MatchState state;
	bool client_sent;
};
