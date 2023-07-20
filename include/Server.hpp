/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 17:38:44 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/20 16:34:49 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <map>
#include <vector>

#include "ClientsManager.hpp"
#include "Message.hpp"

#define POLL_TIMEOUT 1000
#define BACKLOG 10

class Server // NOLINT
{
  public:
	Server(int port);
	~Server();

	int poll();
	void stop();

  private:
	ClientsManager m_clients;
	std::vector<Channel> m_channels;

	static void logReceivedMessage(const Message& message, int fd);
};

#endif
