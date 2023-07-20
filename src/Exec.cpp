/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 15:20:32 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/20 22:59:15 by lbesnard         ###   ########.fr       */
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

	ret.insert(std::make_pair("JOIN", &Exec::join));
	ret.insert(std::make_pair("NICK", &Exec::nick));
	ret.insert(std::make_pair("KICK", &Exec::kick));
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

	const ChannelsIt channelIt =
		findChannel(channels, message.parameters().front());
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
		// TODO: ERR_ALREADYREGISTERED "462" --> fd
		return 0;
	}
	client.second.setUsername(parameters[0].c_str());
	client.second.setRealname(parameters[3].c_str());

	return 0;
}

bool Exec::isNicknameValid(const std::string& str)
{
	if (str[0] == ':' || str[0] == '#')
		return false;
	for (size_t i = 0; i != str.size(); i++)
	{
		if (isspace(str[i]))
			return false;
	}

	return true;
}

int Exec::nick(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	(void)channels;
	std::vector<std::string> params = message.parameters();
	const std::string nickname = params.front();
	if (nickname.empty())
	{
		// TODO ERR_NONICKNAMEGIVEN (431);
		return 1;
	}

	if (!isNicknameValid(nickname))
	{
		// TODO ERR_ERRONEUSNICKNAME "432";
		return 2;
	}

	FdClient& client = clients.get(fd);
	if (clients.isNicknameUsed(nickname.c_str()))
	{
		// TODO ERR_NICKNAMEINUSE (433);
		return 3;
	}
	client.second.setNickname(nickname.c_str());
	return 0;
}

std::vector<std::string> Exec::splitChar(const std::string& str, char c)
{
	std::vector<std::string> ret;
	std::stringstream stream(str);
	std::string token;

	while (std::getline(stream, token, c))
		ret.push_back(token);

	return ret;
}

int Exec::kick(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const FdClient& client = clients.get(fd);

	std::vector<std::string> params = message.parameters();
	if (params.size() < 2)
	{
		// TODO ERR_NEEDMOREPARAMS (461);
		return 1;
	}

	std::vector<std::string> toKickFrom = splitChar(params.front(), ',');
	std::vector<std::string> passwords = splitChar(params[1], ',');
	ChannelsIt tmpChan;
	for (std::vector<std::string>::iterator channelIt = toKickFrom.begin();
		 channelIt != toKickFrom.end(); channelIt++)
	{
		tmpChan = findChannel(channels, *channelIt);
		if (tmpChan != channels.end())
		{
			// TODO ERR_NOSUCHCHANNEL (403);
			continue;
		}
		for (std::vector<std::string>::iterator nickIt = passwords.begin();
			 nickIt != passwords.end(); nickIt++)
		{
			if (tmpChan->isUser(client))
			{
				// TODO ERR_NOTONCHANNEL (442);
				break;
			}
			if (tmpChan->isOperator(client))
			{
				// TODO ERR_CHANOPRIVSNEEDED "482";
				break;
			}
			if (tmpChan->isUser(clients.get((*nickIt).c_str())))
			{
				// TODO ERR_USERNOTINCHANNEL "441";
				continue;
			}
			tmpChan->kick(clients.get((*nickIt).c_str()));
			// TODO send to every user in tmpChan the ban message with optional
			// reasons
		}
	}

	return 0;
}

int Exec::join(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	FdClient& client = clients.get(fd);

	const std::vector<std::string>& params = message.parameters();
	if (params.size() < 2)
	{
		// TODO ERR_NEEDMOREPARAMS (461);
		return 1;
	}
	std::vector<std::string> toJoin = splitChar(params.front(), ',');
	std::vector<std::string> passwords = splitChar(params[1], ',');
	for (size_t i = 0; i != toJoin.size(); i++)
	{
		const ChannelsIt tmpChan = findChannel(channels, toJoin[i]);
		if (tmpChan != channels.end())
		{
			// TODO ERR_NOSUCHCHANNEL (403);
			continue;
		}
		switch (tmpChan->add(client, passwords[i].c_str()))
		{
		case Channel::SUCCESS:
			/*TODO
			~ send to client and all in the channel "<name> JOIN <channel>"
			~ may send a MODE msg with current channel MODE
			~ Sends them RPL_TOPIC and RPL_TOPICTIME numerics if the channel has
			a topic set (if the topic is not set, the user is sent no numerics)
			~ Sends them one or more RPL_NAMREPLY numerics
					(which also contain the name of the user that’s joining)*/
			break;
		case Channel::USER_ALREADY:
			// TODO no idea
			break;
		case Channel::WRONG_PASSWORD:
			// TODO ERR_BADCHANNELKEY "475"
			break;
		case Channel::CHANNELISFULL:
			// TODO ERR_CHANNELISFULL "471"
			break;
		case Channel::INVITEONLYCHAN:
			// TODO ERR_INVITEONLYCHAN "473"
			break;
		}
	}
	return 0;
}

int Exec::part(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	FdClient& client = clients.get(fd);

	const std::vector<std::string>& params = message.parameters();
	if (params.empty())
	{
		// TODO ERR_NEEDMOREPARAMS (461);
		return 1;
	}
	std::vector<std::string> toLeave = splitChar(params.front(), ',');
	for (size_t i = 0; i != toLeave.size(); i++)
	{
		const ChannelsIt tmpChan = findChannel(channels, toLeave[i]);
		if (tmpChan != channels.end())
		{
			// TODO ERR_NOSUCHCHANNEL (403);
			continue;
		}
		if (tmpChan->kick(client))
		{
			// TODO send "<name> leaving the channel <channel>" to everybody in
			// channel
		}
		else
		{
			// TODO ERR_NOTONCHANNEL (442)
		}
	}
	return 0;
}
