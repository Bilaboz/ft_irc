/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 20:21:58 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/20 17:36:41 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <arpa/inet.h>

#include <sstream>
#include <stdexcept>

#include "Exec.hpp"
#include "Log.hpp"
#include "Message.hpp"

Server::Server(const char* port)
{
	int sock_fd = 0;
	int yes = 1;
	addrinfo hints;

	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	addrinfo* result = NULL;
	if (getaddrinfo(NULL, port, &hints, &result) != 0)
	{
		freeaddrinfo(result);
		throw std::runtime_error(std::strerror(errno));
	}

	addrinfo* tmp = NULL;
	for (tmp = result; tmp != NULL; tmp = tmp->ai_next)
	{
		sock_fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (sock_fd < 0)
			continue;

		// get rid of "address already in use" error message
		if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
		{
			freeaddrinfo(result);
			throw std::runtime_error(std::strerror(errno));
		}

		if (bind(sock_fd, tmp->ai_addr, tmp->ai_addrlen) < 0)
		{
			close(sock_fd);
			continue;
		}

		break ;
	}

	freeaddrinfo(result);

	if (tmp == NULL)
		throw std::runtime_error("Server can't bind to any port");

	if (listen(sock_fd, BACKLOG) == -1)
		throw std::runtime_error(std::strerror(errno));

	m_clients.addListener(sock_fd);

	Log::info() << "Server started and listening on port " << port << '\n';
}

int Server::poll()
{
	std::vector<pollfd>& pfds = m_clients.getPollfds();

	if (::poll(pfds.data(), pfds.size(), POLL_TIMEOUT) == -1)
	{
		Log::error() << "poll(): " << std::strerror(errno) << '\n';
		return 1;
	}

	for (std::vector<pollfd>::iterator it = pfds.begin(); it != pfds.end();
		 it++)
	{
		if (it->revents & POLLIN)
		{
			if (it->fd == pfds[0].fd)
			{
				socklen_t addrlen = 0;
				sockaddr addr;

				const int newFd = accept(it->fd, &addr, &addrlen);
				if (newFd == -1)
				{
					Log::error()
						<< "accept(): " << std::strerror(errno) << '\n';
					return 2;
				}

				char ipStr[INET_ADDRSTRLEN];
				struct sockaddr_in* clientIP =
					reinterpret_cast<struct sockaddr_in*>(&addr);

				inet_ntop(
					clientIP->sin_family, &clientIP->sin_addr, ipStr,
					sizeof(ipStr)
				);

				Log::info() << "New connection from " << ipStr << " on fd "
							<< it->fd << "\n";

				m_clients.add(newFd);
			}
			else
			{
				Client& client = m_clients.get(it->fd).second;
				std::string packet = client.receive(it->fd, m_clients);
				if (!packet.empty())
				{
					Message message(packet);
					logReceivedMessage(message, it->fd);
					Exec::exec(message, m_clients, it->fd, m_channels);
				}
			}
		}
	}
	return 0;
}

void Server::logReceivedMessage(const Message& message, const int fd)
{
	Log::debug() << "Received (fd=" << fd << "): " << message.verb();

	for (size_t i = 0; i < message.parameters().size(); ++i)
		Log::debug() << " [" << message.parameters()[i] << "]";

	Log::debug() << '\n';
}

void Server::stop() {}

Server::~Server() {}
