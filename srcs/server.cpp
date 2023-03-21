/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leng-chu <-chu@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/17 17:51:13 by leng-chu          #+#    #+#             */
/*   Updated: 2023/03/20 20:51:49 by leng-chu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

namespace N_LOG
{
	void	msg(const string &message)
	{
		std::cout << message << std::endl;
	}

	int	ErrorExit(const string &errorMessage)
	{
		msg("ERROR: " + errorMessage);
		exit(1);
		return (1);
	}
}

Server::Server(string ip_address, int port)
	: _port(port), _sockfd(), _clientfd(),
	_serverMsg(buildResponse()), _ip(ip_address),
	_socketAddr(), _socketAddr_len(sizeof(_socketAddr))
{
	_socketAddr.sin_family = AF_INET;
	_socketAddr.sin_port = htons(port);
	_socketAddr.sin_addr.s_addr = inet_addr(_ip.c_str());

	if (startServer() != 0)
	{
		ostringstream ss;
		ss << "Failed to start server with PORT: " << ntohs(_socketAddr.sin_port);
		N_LOG::msg(ss.str());
	}
}

Server::~Server()
{
	closeServer();
}

int	Server::startServer()
{
	int	reuse = 1;
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0)
		return N_LOG::ErrorExit("Cannot create socket");
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
		return N_LOG::ErrorExit("Cannot set socket option");
	if (bind(_sockfd, (sockaddr*)&_socketAddr, _socketAddr_len) < 0)
		return N_LOG::ErrorExit("Cannot connect socket to address");
	return (0);
}

void	Server::closeServer()
{
	close(_sockfd);
	close(_clientfd);
	exit(0);
}

void	Server::startListen()
{
	ostringstream	ss;
	int		bytes;
	char		buffer[BUF_SIZE];
	
	if (listen(_sockfd, 20) < 0)
		N_LOG::ErrorExit("Socket listen failed");
	ss << "\n*** Listening on ADDRESS: " 
		<< inet_ntoa(_socketAddr.sin_addr);
	N_LOG::msg(ss.str());

	// Create an array of pollfd structs,
	// one for the listening socket
	const int	MAX_CLIENTS = 10;
	struct pollfd	fds[MAX_CLIENTS + 1];
	memset(fds, 0, sizeof(fds));
	fds[0].fd = _sockfd;
	fds[0].events = POLLIN;

	int nfds = 1; // Number of active socket fds.
	while (1)
	{
		int rv = poll(fds, nfds, -1);
		if (rv == -1)
			N_LOG::ErrorExit("poll() failed");

		// Check if there is a new connection waiting on ther listening
		if (fds[0].revents & POLLIN)
		{
			acceptConnection(_clientfd);
			fds[nfds].fd = _clientfd;
			fds[nfds].events = POLLIN;
			nfds++;
			N_LOG::msg("--- New client connected ---");
		}

		for (int i = 1; i < nfds; i++)
		{
			if (fds[i].revents & POLLIN)
			{
				bzero(buffer, BUF_SIZE);
				bytes = recv(fds[i].fd, buffer, BUF_SIZE, 0);
				if (bytes < 0)
					N_LOG::ErrorExit("Failed to read bytes from client socket connection");
				N_LOG::msg("--- Received Request from client ---");
				sendResponse(fds[i].fd);
				close(fds[i].fd);

				// Remove the client socket from the array
				nfds--;
				fds[i] = fds[nfds];
				i--;
			}
		}
	}
}

void	Server::acceptConnection(int &new_client)
{
	new_client = accept(_sockfd, (sockaddr*)&_socketAddr, &_socketAddr_len);
	if (new_client < 0)
	{
		ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(_socketAddr.sin_addr) << "; PORT: " << ntohs(_socketAddr.sin_port);
		N_LOG::ErrorExit(ss.str());
	}
}

string Server::buildResponse()
{
	string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
	ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n" << htmlFile;
	return ss.str();
}

void Server::sendResponse(int client_fd)
{
	long	bytesSent;

	bytesSent = send(client_fd, _serverMsg.c_str(), _serverMsg.size(), 0);
	if (bytesSent == (long)_serverMsg.size())
		N_LOG::msg("------ Server Response sent to client -----\n\n");
	else
		N_LOG::msg("Error sending response to client");
}
