/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 20:21:58 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/19 18:38:21 by lbesnard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port) : m_port(port)
{
	int sock_fd = 0;
	int yes = 1;
	struct addrinfo hints;

	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* result = NULL;
	if (getaddrinfo(NULL, "1111", &hints, &result) != 0)
	{
		Log::error() << "getaddrinfo(): " << std::strerror(errno) << std::endl;
		exit(1);
	}

	struct addrinfo* tmp = NULL;
	for (tmp = result; tmp != NULL; tmp = tmp->ai_next)
	{
		sock_fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (sock_fd < 0)
			continue;

		// get rid of "address already in use" error message
		setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(sock_fd, tmp->ai_addr, tmp->ai_addrlen) < 0)
		{
			close(sock_fd);
			continue;
		}
	}

	freeaddrinfo(result);

	if (tmp == NULL)
		exit(-1);

	if (listen(sock_fd, BACKLOG) == -1)
		exit(-1);

	m_clients.addListener(sock_fd);
}

int Server::poll()
{
	std::vector<pollfd>& pfds = m_clients.getPollfds();

	if (::poll(pfds.data(), pfds.size(), POLL_TIMEOUT) == -1)
	{
		Log::error() << "poll(): " << std::strerror(errno) << std::endl;
		return (-1);
	}

	for (std::vector<pollfd>::iterator it = pfds.begin(); it != pfds.end();
		 it++)
	{
		if (it->revents & POLLIN)
		{
			if (it->fd == pfds[0].fd)
			{
				// TODO add addr to client class
				// TODO: make this a function e.g acceptNewClient()
				socklen_t addrlen = 0;
				sockaddr addr;

				int newFd = accept(it->fd, &addr, &addrlen);
				if (newFd == -1)
					Log::error()
						<< "accept(): " << std::strerror(errno) << std::endl;
				else
					m_clients.add(newFd);
			}
			else
			{
				receive() // TODO recev and exec
			}
		}
	}
	return 0;
}

void Server::stop() {}

Server::~Server() {}

Server::Server() : m_port(0) {}

Server::Server(const Server& other) : m_port(0) {}

Server& Server::operator=(const Server& rhs) {}
