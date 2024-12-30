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

struct UdpId
{
	WIN_NO_COPY_MOVE(UdpId);

	UdpId()
		: initialized(false)
		, len(sizeof(sockaddr_storage))
	{
		memset(&storage, 0, sizeof(storage));
	}

	bool initialized;
	sockaddr_storage storage;
	socklen_t len;
};

class UdpServer
{
	WIN_NO_COPY_MOVE(UdpServer);

public:
	UdpServer();
	explicit UdpServer(unsigned short port);
	~UdpServer();

	operator bool() const;
	void close();
	void send(const void *buffer, int len, const UdpId &id);
	int recv(void *buffer, int len, UdpId &id);
	unsigned peek();
	bool error() const;

private:
	bool bind(unsigned short port);

	int sock;
};

}
