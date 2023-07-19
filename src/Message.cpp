/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcarles <rcarles@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 15:10:48 by rcarles           #+#    #+#             */
/*   Updated: 2023/07/19 17:38:54 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

#include <iostream>

Message::Message() {}

Message::~Message() {}

Message::Message(const Message& other)
	: m_verb(other.m_verb), m_parameters(other.m_parameters)
{
}

Message::Message(const std::string& msg)
{
	this->parse(msg);
}

void Message::parse(const std::string& str)
{
	size_t start = 0;

	for (size_t i = 0; i <= str.size(); ++i)
	{
		// the verb hasn't been parsed already when start == 0;
		if ((str[i] == ' ' || i == str.size()) && start == 0)
		{
			m_verb = str.substr(start, i);
			toUpper(m_verb);
			start = i + 1;
		}
		// normal parameter (not trailing)
		else if ((str[i] == ' ' || i == str.size()) && str[start] != ':')
		{
			m_parameters.push_back(str.substr(start, i - start));
			start = i + 1;
		}

		// trailing parameter
		if (i != str.size() && str[start] == ':')
		{
			m_parameters.push_back(str.substr(start + 1));
			return;
		}
	}
}

const std::string& Message::verb() const
{
	return m_verb;
}

const std::vector<std::string>& Message::parameters() const
{
	return m_parameters;
}

Message& Message::operator=(const Message& rhs)
{
	if (this != &rhs)
	{
		m_verb = rhs.m_verb;
		m_parameters = rhs.m_parameters;
	}

	return *this;
}

void Message::toUpper(std::string& str)
{
	for (size_t i = 0; i < str.size(); ++i)
		str[i] = std::toupper(str[i]);
}
