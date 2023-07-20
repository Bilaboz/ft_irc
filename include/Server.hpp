/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 17:38:44 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/19 18:39:24 by lbesnard         ###   ########.fr       */
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
#include "Log.hpp"

#define POLL_TIMEOUT 1000
#define BACKLOG 10

class Server
{
  public:
	Server(int port);
	int poll();
	void stop();
	~Server();

  private:
	const int m_port;
	ClientsManager m_clients;
	//	std::vector<Channel> m_channels;

	Server();
	Server(const Server& other);
	Server& operator=(const Server& rhs);
};

#endif
