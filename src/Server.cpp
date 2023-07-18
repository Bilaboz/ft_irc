/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 20:21:58 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/18 14:44:50 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port) : m_port(port) {}

int Server::poll()
{
	return 1;
}

void Server::stop() {}

Server::~Server() {}

Server::Server() : m_port(0) {}

Server::Server(const Server& other) : m_port(0)
{
	(void)other;
}

Server& Server::operator=(const Server& rhs)
{
	(void)rhs;
	return *this;
}
