/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 16:42:18 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/18 17:43:57 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

#include <algorithm>
#include <vector>

#include "Client.hpp"

Channel::Channel() {}

Channel::Channel(const char* name) : m_name(name) {}

Channel::Channel(const Channel& other)
	: inviteOnly(other.inviteOnly), topicOperator(other.topicOperator),
	  m_name(other.m_name), m_users(other.m_users),
	  m_operators(other.m_operators), m_invited(other.m_invited),
	  m_password(other.m_password), m_userLimit(other.m_userLimit)
{
}

Channel& Channel::operator=(const Channel& rhs)
{
	inviteOnly = rhs.inviteOnly;
	topicOperator = rhs.topicOperator;
	m_name = rhs.m_name;
	m_users = rhs.m_users;
	m_operators = rhs.m_operators;
	m_invited = rhs.m_invited;
	m_password = rhs.m_password;
	m_userLimit = rhs.m_userLimit;

	return *this;
}

Channel::~Channel() {}

int Channel::kick(FdClient& user)
{
	bool erased = false;
	std::vector<FdClient*>::iterator it =
		std::find(m_users.begin(), m_users.end(), &user);

	if (it != m_users.end())
	{
		m_users.erase(it);
		erased = true;
	}

	it = std::find(m_operators.begin(), m_operators.end(), &user);
	if (it != m_operators.end())
	{
		m_operators.erase(it);
		erased = true;
	}

	it = std::find(m_invited.begin(), m_invited.end(), &user);
	if (it != m_invited.end())
	{
		m_invited.erase(it);
		erased = true;
	}

	return erased ? SUCCESS : USER_NOEXIST;
}

int Channel::add(FdClient& user, const char* password)
{
	std::vector<FdClient*>::iterator it =
		std::find(m_users.begin(), m_users.end(), &user);

	if (it == m_users.end())
	{
		if (m_password != password)
			return WRONG_PASSWORD;
		m_users.push_back(&user);
		return SUCCESS;
	}

	return USER_ALREADY;
}

int Channel::promote(FdClient& user)
{
	if (!this->isUser(user))
		return USER_NOEXIST;

	const std::vector<FdClient*>::iterator it =
		std::find(m_operators.begin(), m_operators.end(), &user);

	if (it == m_operators.end())
	{
		m_users.push_back(&user);
		return SUCCESS;
	}

	return USER_ALREADY;
}

int Channel::retrograde(FdClient& user)
{
	if (!this->isUser(user))
		return USER_NOEXIST;

	const std::vector<FdClient*>::iterator it =
		std::find(m_operators.begin(), m_operators.end(), &user);

	if (it != m_operators.end())
	{
		m_users.erase(it);
		return SUCCESS;
	}

	return USER_ALREADY;
}

// void Channel::msg(char* msg, const FdClient& sender) {}

bool Channel::isOperator(const FdClient& user) const
{
	const std::vector<FdClient*>::const_iterator it =
		std::find(m_operators.begin(), m_operators.end(), &user);

	return it != m_operators.end();
}

bool Channel::isUser(const FdClient& user) const
{
	const std::vector<FdClient*>::const_iterator it =
		std::find(m_users.begin(), m_users.end(), &user);

	return it != m_users.end();
}
