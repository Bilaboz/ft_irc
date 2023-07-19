/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 18:15:47 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/19 20:56:01 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {}

Client::Client(const Client& other)
	: m_nickname(other.m_nickname), m_username(other.m_username), buf(other.buf)
{
}

Client& Client::operator=(const Client& rhs)
{
	if (this != &rhs)
	{
		m_nickname = rhs.m_nickname;
		m_username = rhs.m_username;
		buf = rhs.buf;
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
