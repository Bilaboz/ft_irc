/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 16:42:18 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/22 21:06:17 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

#include <algorithm>
#include <vector>

#include "Client.hpp"
#include "Exec.hpp"

Channel::Channel() {}

Channel::Channel(const char* name) : m_name(name) {}

Channel::Channel(const Channel& other)
	: inviteOnly(other.inviteOnly), isTopicProtected(other.isTopicProtected),
	  m_name(other.m_name), m_users(other.m_users),
	  m_operators(other.m_operators), m_invited(other.m_invited),
	  m_password(other.m_password), m_userLimit(other.m_userLimit)
{
}

Channel& Channel::operator=(const Channel& rhs)
{
	inviteOnly = rhs.inviteOnly;
	isTopicProtected = rhs.isTopicProtected;
	m_name = rhs.m_name;
	m_users = rhs.m_users;
	m_operators = rhs.m_operators;
	m_invited = rhs.m_invited;
	m_password = rhs.m_password;
	m_userLimit = rhs.m_userLimit;

	return *this;
}

Channel::~Channel() {}

void Channel::invite(FdClient& user)
{
	m_invited.push_back(&user);
}

int Channel::kick(FdClient& user, std::vector<Channel>& channels)
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

	if (m_users.empty())
	{
		std::vector<Channel>::iterator itchan(this);
		channels.erase(itchan);
	}
	return erased ? SUCCESS : USER_NOEXIST;
}

int Channel::add(FdClient& user, const char* password)
{
	const std::vector<FdClient*>::iterator it =
		std::find(m_users.begin(), m_users.end(), &user);

	if (it == m_users.end())
	{
		if (m_password != password)
			return WRONG_PASSWORD;
		if (m_users.size() == (size_t)m_userLimit)
			return CHANNELISFULL;
		if (inviteOnly && !isInvited(user))
			return INVITEONLYCHAN;
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

bool Channel::isInvited(const FdClient& user) const
{
	const std::vector<FdClient*>::const_iterator it =
		std::find(m_invited.begin(), m_invited.end(), &user);

	return it != m_invited.end();
}

std::string Channel::getName() const
{
	return m_name;
}

std::vector<FdClient*> Channel::getUsers() const
{
	return m_users;
}

std::string Channel::getTopic() const
{
	return m_topic;
}

void Channel::setTopic(const std::string& topic)
{
	m_topic = topic;
}

void Channel::send(
	const FdClient& author, const std::string& message, bool includeSource,
	bool sendToAuthor
)
{
	std::string source;
	if (includeSource)
		source = author.second.getSource();

	for (std::vector<FdClient*>::iterator it = m_users.begin();
		 it != m_users.end(); ++it)
	{
		if (&author != *it || sendToAuthor)
			Exec::sendToClient(**it, source + message);
	}
}
