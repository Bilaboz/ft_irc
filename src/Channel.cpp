/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 16:42:18 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/14 14:17:46 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

#include <vector>

#include "Client.hpp"

bool inviteOnly;
bool topicOperator;
std::string m_name;
std::vector<Client*> m_users;
std::vector<Client*> m_operators;
std::vector<Client*> m_invited;
std::string m_password;
int m_userLimit;

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

int Channel::kick(FdClient& user) {}

int Channel::add(FdClient& user, const char* str) {}

int Channel::promote(FdClient& user) {}

int Channel::retrograde(FdClient& user) {}

void Channel::msg(char* msg, const FdClient& sender) {}

bool Channel::isOperator(const FdClient& user)
{
	for (std::vector<FdClient*>::const_iterator
}

bool Channel::isUser(FdClient& user) {}
