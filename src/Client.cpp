/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 18:15:47 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/20 16:03:59 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>

#include "ClientsManager.hpp"

Client::Client() {}

Client::Client(const Client& other)
	: m_nickname(other.m_nickname), m_username(other.m_username),
	  m_buffer(other.m_buffer)
{
}

Client& Client::operator=(const Client& rhs)
{
	if (this != &rhs)
	{
		m_nickname = rhs.m_nickname;
		m_username = rhs.m_username;
		m_buffer = rhs.m_buffer;
	}

	return *this;
}

Client::~Client() {}

const std::string& Client::getUsername() const
{
	return m_username;
}

const std::string& Client::getNickname() const
{
	return m_nickname;
}

const std::string& Client::getRealname() const
{
	return m_realname;
}

void Client::setUsername(const char* username)
{
	m_username = username;
}

void Client::setNickname(const char* nickname)
{
	m_nickname = nickname;
}

void Client::setRealname(const char* realname)
{
	m_realname = realname;
}

std::string Client::receive(int fd, ClientsManager& clients)
{
	if (m_buffer.receive(fd) == PacketBuffer::CLIENT_DISCONNECTED ||
		errno == ECONNRESET)
		clients.remove(fd);

	return m_buffer.getPacket();
}
