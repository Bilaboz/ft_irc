/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 17:38:44 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/14 14:22:23 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <poll.h>
#include "ClientsManager.hpp"

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
