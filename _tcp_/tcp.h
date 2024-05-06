#pragma once
#if defined (_WIN32) // Guard for future support of POSIX sockets in our code
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <stdio.h>
#include <stdlib.h>

// To ensure portability on non-Windows systems
// for the SOCKET data type on the Windows OS
#if !defined (_WIN32)
#define SOCKET int
#endif

// For invalid sockets on non-Windows systems
#if defined (_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#endif

// For portability when closing sockets on Windows
// and non-Windows systems
#if !defined(_WIN32)
#define CLOSESOCKET(s) close(s)
#else
#define CLOSESOCKET(s) closesocket(s)
#endif

// Error handling when dealing with other socket functions
#if defined(_WIN32)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define GETSOCKETERRNO() (errno)
#endif
