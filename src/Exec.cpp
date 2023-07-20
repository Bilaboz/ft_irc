/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 15:20:32 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/20 17:57:15 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exec.hpp"

#include <map>
#include <utility>

#include "Client.hpp"

const std::map<std::string, Exec::func> Exec::m_functions = Exec::initTable();

int Exec::exec(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const func function = m_functions.at(message.verb());
	return function(message, clients, fd, channels);
}

std::map<std::string, Exec::func> Exec::initTable()
{
	std::map<std::string, func> ret;

	//ret.insert(std::make_pair("JOIN", &Exec::join));
	//ret.insert(std::make_pair("NICK", &Exec::nick));
	//ret.insert(std::make_pair("KICK", &Exec::kick));
	ret.insert(std::make_pair("USER", &Exec::user));
	ret.insert(std::make_pair("TOPIC", &Exec::topic));

	return ret;
}

Exec::ChannelsIt Exec::findChannel(
	std::vector<Channel>& channels, const std::string& name
)
{
	for (ChannelsIt it = channels.begin(); it != channels.end(); ++it)
	{
		if (it->getName() == name)
			return it;
	}

	return channels.end();
}

int Exec::topic(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	if (message.parameters().empty())
	{
		// ERR_NEEDMOREPARAMS (461)
		return 0;
	}

	ChannelsIt channelIt = findChannel(channels, message.parameters().front());
	if (channelIt == channels.end())
	{
		// ERR_NOSUCHCHANNEL (403)
		return 0;
	}

	if (message.parameters().size() == 1)
	{
		// RPL_TOPIC (332) or RPL_NOTOPIC (331)
		// maybe then RPL_TOPICWHOTIME (333)
		return 0;
	}

	if (channelIt->isTopicProtected && !channelIt->isOperator(clients.get(fd)))
	{
		// ERR_CHANOPRIVSNEEDED (482)
		return 0;
	}

	channelIt->setTopic(message.parameters()[1]);
	// broadcast RPL_TOPIC (332) or RPL_NOTOPIC (331)
	// then RPL_TOPICWHOTIME (333)
	return 0;
}

int Exec::user(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	(void)channels;
	const std::vector<std::string>& parameters = message.parameters();
	if (parameters.size() != 4 || parameters[3].empty())
	{
		// TODO: ERR_NEEDMOREPARAMS (461) --> fd
		return 0;
	}

	FdClient& client = clients.get(fd);
	if (!client.second.getUsername().empty())
	{
		// TODO: ERR_ALREADYREGISTERED (462) --> fd
		return 0;
	}
	client.second.setUsername(parameters[0].c_str());
	client.second.setRealname(parameters[3].c_str());

	return 0;
}
