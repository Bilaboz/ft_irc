/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 16:42:18 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/28 17:58:51 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

#include <algorithm>
#include <ctime>
#include <sstream>
#include <vector>

#include "Client.hpp"
#include "Exec.hpp"
#include "Log.hpp"

Channel::Channel() {}

Channel::Channel(const char* name)
	: inviteOnly(), isTopicProtected(), m_name(name), m_userLimit()
{
}

Channel::Channel(const Channel& other)
	: inviteOnly(other.inviteOnly), isTopicProtected(other.isTopicProtected),
	  m_name(other.m_name), m_topic(other.m_topic), m_users(other.m_users),
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
	m_topic = rhs.m_topic;
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
	Log::info() << user.second.getNickname() << " has been invited to " << m_name << '\n';
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
		const std::vector<Channel>::iterator itchan(this);
		channels.erase(itchan);
	}

	if (erased)
		Log::info() << user.second.getNickname() << " left (or kicked) " << m_name
					<< '\n';

	return erased ? SUCCESS : USER_NOEXIST;
}

int Channel::add(FdClient& user, const char* password)
{
	const std::vector<FdClient*>::iterator it =
		std::find(m_users.begin(), m_users.end(), &user);

	if (it == m_users.end())
	{
		if (!isInvited(user))
		{
			if (!m_password.empty() && m_password != password)
				return WRONG_PASSWORD;
			if (m_userLimit && m_users.size() >= (size_t)m_userLimit)
				return CHANNELISFULL;
			if (inviteOnly)
				return INVITEONLYCHAN;
		}

		m_users.push_back(&user);

		Log::info() << user.second.getNickname() << " joined channel " << m_name << '\n';

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
		m_operators.push_back(&user);
		Log::info() << user.second.getNickname()
					<< " gained operator privileges for channel " << m_name << '\n';
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
		m_operators.erase(it);
		Log::info() << user.second.getNickname()
					<< " lost operator privileges for channel " << m_name << '\n';
		return SUCCESS;
	}

	return USER_ALREADY;
}

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

void Channel::setTopic(const std::string& topic, const std::string& setterNick)
{
	std::stringstream stream;
	stream << std::time(NULL);
	m_topicTimestamp = stream.str();
	m_topic = topic;
	m_topicSetter = setterNick;
}

const std::string& Channel::getTopicTimestamp() const
{
	return m_topicTimestamp;
}

const std::string& Channel::getTopicSetter() const
{
	return m_topicSetter;
}

void Channel::send(
	const FdClient& author, const std::string& message, bool includeSource,
	bool sendToAuthor
)
{
	std::string source;
	if (includeSource)
		source = author.second.getSource() + " ";

	for (std::vector<FdClient*>::iterator it = m_users.begin(); it != m_users.end(); ++it)
	{
		if (&author != *it || sendToAuthor)
			Exec::sendToClient(**it, source + message);
	}
}

void Channel::setPassword(const char* password)
{
	m_password = password;
}

std::string Channel::getModes() const
{
	std::string modes = "+n";
	if (inviteOnly)
		modes += 'i';

	if (isTopicProtected)
		modes += 't';

	if (!m_password.empty())
		modes += 'k';

	if (modes == "+")
		return "";

	return modes;
}

void Channel::setUserLimit(int limit)
{
	m_userLimit = limit;
}
