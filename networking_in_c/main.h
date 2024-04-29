#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif // !_WIN32_WINNT

#include <stdio.h>
#include <winsock2.h> //
#include <WS2tcpip.h> // Headers needed to list network adapters on windows
#include <iphlpapi.h> //
#include <stdlib.h>


#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")


// functions

/**
* @brief Prints the address information of each adapter
* @param pAdapterAddr - A pointer to a list of adapters
*/
void PrintAdaptersInformation(PIP_ADAPTER_ADDRESSES pAdapterAddr);
