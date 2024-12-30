#pragma once

#include <win/Win.hpp>

#undef distance
#include <string>
#include <cstring>
#ifdef _WIN32
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sys/types.h>
#define ssize_t SSIZE_T
#else
#include <netinet/in.h>
#include <netdb.h>
#endif

namespace win {

#ifdef _WIN32
	constexpr int WOULDBLOCK = WSAEWOULDBLOCK;
	constexpr int CONNRESET = WSAECONNRESET;
#else
	constexpr int WOULDBLOCK = EWOULDBLOCK;
	constexpr int CONNRESET = ECONNRESET;
#endif

class UdpClient
{
	WIN_NO_COPY_MOVE(UdpClient);

public:
	UdpClient();
	UdpClient(const std::string &address, unsigned short port);
	~UdpClient();

	operator bool() const;
	void close();
	void send(const void *buffer, unsigned len);
	int recv(void *buffer, unsigned len);
	unsigned peek();
	bool error() const;

private:
	int sock;
	addrinfo *ai;
};

}
