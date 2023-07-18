/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 17:38:44 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/18 14:33:38 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>

#include <map>
#include <vector>

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
