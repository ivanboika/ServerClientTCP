#pragma once

#include <iostream>
#include <stdio.h>
#include <winsock2.h> 

#include <windows.h>
#include <vector>
#include <algorithm>
#define MY_PORT 666

DWORD WINAPI WorkWithClient(LPVOID client_socket);

int nclients = 0;

int start1(int argc, char* argv[])
{
	char buff[1024]; 

	if (WSAStartup(0x0202, (WSADATA*)&buff[0]))
	{
		WSAGetLastError();
		return-1;
	}

	SOCKET mysocket;

	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		WSACleanup();
		return -1;
	}

	sockaddr_in local_addr;

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	// не забываем о сетевом порядке!!!
	local_addr.sin_addr.s_addr = 0;
	// сервер принимает подключения
	// на все IP

	if (bind(mysocket, (sockaddr*)&local_addr, sizeof(local_addr)))
	{
		closesocket(mysocket); 
		WSACleanup();
		return -1;
	}

	std::cout << "\nServer successfully started!";
	if (listen(mysocket, 0x100))
	{
		closesocket(mysocket);
		WSACleanup();
		return-1;
	}

	SOCKET client_socket; 

	sockaddr_in client_addr; 

	int client_addr_size = sizeof(client_addr);

	while ((client_socket = accept(mysocket, (sockaddr*)&client_addr, &client_addr_size)))
	{
		std::cout << "\nClient has connected!";
		nclients++; 
		HOSTENT* hst;
		hst = gethostbyaddr((char*)
			&client_addr.sin_addr
			.s_addr, 4, AF_INET);


		std::cout << "\nClient info: " << (hst) ? hst->h_name : "", inet_ntoa(client_addr.sin_addr);

		DWORD thID;
		CreateThread(NULL, NULL, WorkWithClient,
			&client_socket, NULL, &thID);
	}
	return 0;
}

DWORD WINAPI WorkWithClient(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET*)client_socket)[0];
	char buff[20 * 1024];
	#define sHELLO "Hello, Student!\r\n"

	send(my_sock, sHELLO, sizeof(sHELLO), 0);

	int bytes_recv = 0;
	while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR)
	{		
		std::string message = buff;
		std::string temp{ "" };
		std::cout << "\nMessage from client: " << buff;

		auto includesOrNot = message.find('.');
		auto endConnection = message.find("..");
		
		if (endConnection != std::string::npos)
		{
			std::cout << "\nFound end connection symbols. Closing connection...";
			closesocket(my_sock);
			std::cout << "\nClient has disconnected";
			--nclients;
		}

		
		if ((includesOrNot != std::string::npos) && ((message[message.size()-1]) == '.'))
		{
			std::cout << "\nPoint at the end has found!";

			std::vector<std::string> words;
			

			for (size_t i{ 0 }; i < message.size(); ++i)
			{
				if (message[i] != ' ' && message[i] != '.')
				{
					temp += message[i];
				}
				else
				{
					if (!temp.empty())
					{
						words.emplace_back(temp);
						temp.clear();
					}
				}
			}

			temp.clear();

			for (int i = words.size() - 1; i >= 0; --i)
			{
				temp += words[i];
				temp += " ";
			}
			message = temp;			
		}

		message += '\n';
		message += '\0';
		send(my_sock, message.c_str(), message.size(), 0);
	}

	--nclients;
	closesocket(my_sock);
	std::cout << "\nClient has disconnected";
	return 0;
}