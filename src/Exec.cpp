/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 15:20:32 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/19 18:46:09 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exec.hpp"

#include <map>
#include <utility>
#include "Client.hpp"

const std::map<std::string, Exec::func> Exec::m_functions = Exec::initTable();

int Exec::exec(
	const Message& Message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const func function = m_functions.at(Message.verb());
	return function(Message, clients, fd, channels);
}

std::map<std::string, Exec::func> Exec::initTable()
{
	std::map<std::string, func> ret;

	ret.insert(std::make_pair("JOIN", &Exec::join));
	ret.insert(std::make_pair("NICK", &Exec::nick));
	ret.insert(std::make_pair("KICK", &Exec::kick));

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
	
int Exec::user(const Message& message, ClientsManager& clients, int fd, std::vector<Channel>& channels)
{
	(void)channels;
	const std::vector<std::string>& parameters = message.parameters();
	if (parameters.size() != 4 || parameters[3].empty())
	{
		//TODO: ERR_NEEDMOREPARAMS (461) --> fd
		//TODO: return
	}

	FdClient& client = clients.get(fd);
	if (client.second.getUsername().empty())
	{
		//TODO: ERR_ALREADYREGISTERED (462) --> fd
		//TODO: return
	}

	return 0;
}
