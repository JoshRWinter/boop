#pragma once

#include <win/Win.hpp>

namespace win
{

#ifdef WINPLAT_WINDOWS
static struct WinWsaInit
{
	WinWsaInit()
	{
		WSADATA wsa;
		WSAStartup(MAKEWORD(1, 1), &wsa);
	}

	~WinWsaInit()
	{
		WSACleanup();
	}
};
#endif

void initialize_wsa();

}
