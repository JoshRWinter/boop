#pragma once

#include <string>

#include <win/Win.hpp>

#include "UdpServer.hpp"
#include "UdpClient.hpp"

class NetworkMatch
{
	WIN_NO_COPY_MOVE(NetworkMatch);

public:
	static constexpr int PORT = 28857;

	NetworkMatch(bool host, const std::string &connect_to_ip);

	// called by host
	bool host_get_data(float &guest_paddle_y);
	void host_send_data(float host_paddle_y, float ball_x, float ball_y, int host_score, int guest_score);

	// called by guest
	bool guest_get_data(float &host_paddle_y, float &ball_x, float &ball_y, int &host_score, int &guest_score);
	void guest_send_data(float guest_paddle_y);

private:
	win::UdpServer server;
	win::UdpClient client;
	win::UdpId guestid;

	bool data_available;
};
