/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 18:15:47 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/27 20:11:11 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>
#include <sstream>

#include "ClientsManager.hpp"

Client::Client() : hasSentPassword(false), isRegistered(false), isNetworkOperator(false)
{
}

Client::Client(const Client& other)
	: hasSentPassword(other.hasSentPassword), isRegistered(other.isRegistered),
	  isNetworkOperator(other.isNetworkOperator), m_nickname(other.m_nickname),
	  m_username(other.m_username), m_host(other.m_host), m_buffer(other.m_buffer)
{
}

Client& Client::operator=(const Client& rhs)
{
	if (this != &rhs)
	{
		hasSentPassword = rhs.hasSentPassword;
		isRegistered = rhs.isRegistered;
		isNetworkOperator = rhs.isNetworkOperator;
		m_nickname = rhs.m_nickname;
		m_username = rhs.m_username;
		m_host = rhs.m_host;
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

const std::string& Client::getHost() const
{
	return m_host;
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

void Client::setHost(const char* host)
{
	m_host = host;
}

std::string Client::receive(
	int fd, ClientsManager& clients, std::vector<Channel>& channels, bool newPollin
)
{
	if (newPollin)
	{
		if (m_buffer.receive(fd) == PacketBuffer::CLIENT_DISCONNECTED ||
			errno == ECONNRESET)
		{
			clients.remove(fd, channels);
			return "";
		}
	}

	return m_buffer.getPacket();
}

std::string Client::getSource() const
{
	std::stringstream source;

	source << ':' << m_nickname;

	if (!m_username.empty())
		source << '!' << m_username;

	if (!m_host.empty())
		source << '@' << m_host;

	return source.str();
}
