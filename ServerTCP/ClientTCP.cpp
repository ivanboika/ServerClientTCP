#pragma once

#include <iostream>
#include <stdio.h>
#include<string>
#include<winsock2.h>
#include<windows.h>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <fstream>

int main(int argc, char* argv[])
{
	char buff[1024];

	std::string connectingMessage;
	std::string message;
	std::string address{ "" };
	uint16_t port{ 0 };
	std::ofstream file("Log.txt");

	size_t connectOrNot{};
	do
	{
		std::cout << "connect <address>:<port>\n";

		std::getline(std::cin, connectingMessage);

		connectOrNot = connectingMessage.find("connect");
		connectingMessage.erase(0, sizeof("connect"));

		if (connectingMessage.empty())
		{
			std::cout << "\nNo required params!";
		}
		
		size_t index{ 0 };
		while (index<connectingMessage.size())
		{
			if (connectingMessage[index] != ':')
			{
				address += connectingMessage[index];
				index++;
			}
			else
			{
				connectingMessage.erase(0, index + 1);
				index = MAXINT16;
			}
		}
		
		auto lastParam = connectingMessage.find(' ');
		if (lastParam == std::string::npos && std::stoi(connectingMessage)<65535)
		{
			port = std::stoi(connectingMessage);
		}

		std::cin.clear();

	} while (connectOrNot == std::string::npos);

	if (WSAStartup(0x202, (WSADATA*)&buff[0]))
	{
		return -1;
	}

	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)	
	{
		return -1;
	}

	/*if (argc==3)
	{
		address = argv[1];				
		port = std::stoi(static_cast<std::string>(argv[2]));

		std::cout << "\n\nAddress: " << address << " Port:" << port << std::endl;
	}
	else
	{
		std::cout << "\nWrong connection address or port!";
		closesocket(my_sock);
		return -1;
	}*/

	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	HOSTENT* hst;

	if (inet_addr(address.c_str()) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(address.c_str());
	else if (hst = gethostbyname(address.c_str()))

		((unsigned long*)&dest_addr.sin_addr)[0] =
		((unsigned long**)hst->h_addr_list)[0][0];
	else
	{
		WSACleanup();
		return -1;
	}

	

	if (connect(my_sock, (sockaddr*)&dest_addr, sizeof(dest_addr)))
	{
		std::cout << "\nConnection failed!";
		return -1;
	}
	else
	{
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		file<<"Connection approved: "<< std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.%z%Z") << "\n";
		std::cout << "\nConnection approved";
	}

	int nsize;
	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{
		buff[nsize] = 0;

		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		file << "Message: " << buff << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.%z%Z") << "\n";

		std::cout << "Server message:" << buff;

		std::getline(std::cin, message);

		if ((message.find("disconnect") != std::string::npos))
		{
			std::cout << "\nConnection closed";
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);

			file << "Connection closed: " << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.%z%Z") << "\n";

			Sleep(2000);
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}
		message += '\0';

		// передаем строку клиента серверу
		send(my_sock, message.c_str(), message.size(), 0);
		message.clear();
	}

	std::cout << "\nConnection closed";

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	file << "Connection closed: " << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.%z%Z") << "\n";

	Sleep(2000);
	closesocket(my_sock);
	WSACleanup();
	return -1;
}
