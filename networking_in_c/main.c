#include "main.h"

/**
* @brief Program description: List the network adapters on windows
* @return 0 (success), otherwise -1
*/
int main()
{
	WSADATA d;

	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		printf("Failed to initialize.\n");
		return -1;
	}

	// allocate memory for the network adapters and request the adapters' addrs
	DWORD asize = 20000;
	PIP_ADAPTER_ADDRESSES adapters;
	do
	{
		adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize); // mem allocation

		if (!adapters)
		{
			printf("Couldn't allocate %ld bytes for adapters.\n", asize);
			WSACleanup();
			return -1;
		}

		// requesting for adapters' addrs using GetAdapterAddresses
		int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0, adapters, &asize);
		if (r == ERROR_BUFFER_OVERFLOW)
		{
			printf("GetAdaptersAddresses wants %ld bytes.\n", asize);
			free(adapters);
		}
		else if (r == ERROR_SUCCESS)
		{
			break;
		}
		else
		{
			printf("Error from GetAdaptersAddresses: %d\n", r);
			free(adapters);
			WSACleanup();
			return -1;
		}

	} while (!adapters);

	/**
	* On success, GetAdaptersAddresses writes a linked list into the adapters variables
	* with each adapter's address information. Next step, is to loop through the list
	* and print the information for each adapter.
	*/
	PrintAdaptersInformation(adapters);

	// clean up
	WSACleanup();
	free(adapters);
	printf("Ok.\n");

	return 0;
}

void PrintAdaptersInformation(PIP_ADAPTER_ADDRESSES pAdapterAddr)
{
	PIP_ADAPTER_ADDRESSES adapter = pAdapterAddr;
	while (adapter)
	{
		printf("\nAdapter name: %S\n", adapter->FriendlyName);

		// addresses are also stored as a linked list
		PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;
		while (address)
		{
			printf("\t%s", address->Address.lpSockaddr->sa_family == AF_INET ? "IPv4" : "IPv6");
			char ap[100]; // to store the text repr of the address

			getnameinfo(address->Address.lpSockaddr, address->Address.iSockaddrLength,
				ap, sizeof(ap), 0, 0, NI_NUMERICHOST);	// convert the address into a standard notation address
			printf("\t%s\n", ap);
			address = address->Next;
		}
		adapter = adapter->Next;
	}
}
