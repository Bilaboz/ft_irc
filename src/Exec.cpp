/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 15:20:32 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/22 20:26:52 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exec.hpp"

#include <sys/socket.h>

#include <map>
#include <stdexcept>

#include "Client.hpp"
#include "Log.hpp"
#include "RPL.hpp"

const std::map<std::string, Exec::func> Exec::m_functions = Exec::initTable();

int Exec::exec(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const std::map<std::string, func>::const_iterator func =
		m_functions.find(message.verb());
	if (func != m_functions.end())
		return func->second(message, clients, fd, channels);
	Log::warning() << "Unknown command: [" << message.verb() << "]\n";
	return -1;
}

std::map<std::string, Exec::func> Exec::initTable()
{
	std::map<std::string, func> ret;

	ret.insert(std::make_pair("JOIN", &Exec::join));
	ret.insert(std::make_pair("NICK", &Exec::nick));
	ret.insert(std::make_pair("KICK", &Exec::kick));
	ret.insert(std::make_pair("USER", &Exec::user));
	ret.insert(std::make_pair("TOPIC", &Exec::topic));
	ret.insert(std::make_pair("INVITE", &Exec::invite));

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

void Exec::sendToClient(const FdClient& client, const std::string& message)
{
	size_t bytesSent = 0;
	const std::string data = message + "\r\n";

	while (bytesSent < data.size())
	{
		const ssize_t sent =
			send(client.first, data.c_str() + bytesSent, data.size(), 0);
		if (sent == -1)
		{
			Log::error() << "send(): " << std::strerror(errno) << '\n';
			return;
		}

		bytesSent += sent;
	}
}

int Exec::topic(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const FdClient sender = clients.get(fd);
	const std::string& senderNick = sender.second.getNickname();
	if (message.parameters().empty())
	{
		sendToClient(sender, ERR_NEEDMOREPARAMS(senderNick, "TOPIC"));
		return 0;
	}

	const ChannelsIt channelIt =
		findChannel(channels, message.parameters().front());
	if (channelIt == channels.end())
	{
		sendToClient(
			sender, ERR_NOSUCHCHANNEL(senderNick, message.parameters()[0])
		);
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
		sendToClient(
			sender, ERR_CHANOPRIVSNEEDED(senderNick, channelIt->getName())
		);
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
	FdClient& sender = clients.get(fd);
	const std::string& senderNick = sender.second.getNickname();
	if (parameters.size() < 4 || parameters[3].empty())
	{
		sendToClient(sender, ERR_NEEDMOREPARAMS(senderNick, "USER"));
		return 1;
	}

	if (!sender.second.getUsername().empty())
	{
		sendToClient(sender, ERR_ALREADYREGISTERED(senderNick));
		return 1;
	}

	sender.second.setUsername(parameters[0].c_str());
	sender.second.setRealname(parameters[3].c_str());
	sendToClient(sender, RPL_WELCOME(senderNick, sender.second.getSource()));

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
	FdClient& client = clients.get(fd);
	std::vector<std::string> params = message.parameters();
	if (params.empty())
	{
		sendToClient(
			client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "nick")
		);
		return 1;
	}

	const std::string nickname = params.front();
	if (nickname.empty())
	{
		sendToClient(client, ERR_NONICKNAMEGIVEN(client.second.getNickname()));
		return 1;
	}

	if (!isNicknameValid(nickname))
	{
		sendToClient(
			client, ERR_ERRONEUSNICKNAME(client.second.getNickname(), nickname)
		);
		return 1;
	}

	if (clients.isNicknameUsed(nickname.c_str()))
	{
		sendToClient(
			client, ERR_NICKNAMEINUSE(client.second.getNickname(), nickname)
		);
		return 1;
	}
	client.second.setNickname(nickname.c_str());
	return 0;
}

std::vector<std::string> Exec::splitChar(const std::string& str, char del)
{
	std::vector<std::string> ret;
	std::stringstream stream(str);
	std::string token;

	while (std::getline(stream, token, del))
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
		sendToClient(
			client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "kick")
		);
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
			sendToClient(
				client, ERR_NOSUCHCHANNEL(
							client.second.getNickname(), tmpChan->getName()
						)
			);
			continue;
		}
		for (std::vector<std::string>::iterator nickIt = passwords.begin();
			 nickIt != passwords.end(); nickIt++)
		{
			if (tmpChan->isUser(client))
			{
				sendToClient(
					client, ERR_NOTONCHANNEL(
								client.second.getNickname(), tmpChan->getName()
							)
				);
				break;
			}
			if (tmpChan->isOperator(client))
			{
				sendToClient(
					client, ERR_CHANOPRIVSNEEDED(
								client.second.getNickname(), tmpChan->getName()
							)
				);
				break;
			}
			if (tmpChan->isUser(clients.get((*nickIt).c_str())))
			{
				sendToClient(
					client,
					ERR_USERNOTINCHANNEL(
						client.second.getNickname(), *nickIt, tmpChan->getName()
					)
				);
				continue;
			}
			tmpChan->kick(clients.get((*nickIt).c_str()));
			if (params[2].empty())
				tmpChan->send(
					client, "KICK " + tmpChan->getName() + " " + *nickIt
				);
			else
				tmpChan->send(
					client, "KICK " + tmpChan->getName() + " " + *nickIt +
								" :" + params[2]
				);
		}
	}
	return 0;
}

std::string Exec::usersName(Channel& chan)
{
	std::vector<FdClient*> vec = chan.getUsers();
	std::string ret = ":" + vec[0]->second.getNickname();
	for (size_t i = 1; i != vec.size(); i++)
	{
		std::string tmp = " ";
		if (chan.isOperator(*vec[i]))
			tmp += "@";
		tmp.append(vec[i]->second.getNickname());
		ret.append(tmp);
	}
	return ret;
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
		sendToClient(
			client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "join")
		);
		return 1;
	}
	std::vector<std::string> toJoin = splitChar(params.front(), ',');
	std::vector<std::string> passwords = splitChar(params[1], ',');
	for (size_t i = 0; i != toJoin.size(); i++)
	{
		const ChannelsIt tmpChan = findChannel(channels, toJoin[i]);
		if (tmpChan != channels.end())
		{
			sendToClient(
				client, ERR_NOSUCHCHANNEL(
							client.second.getNickname(), tmpChan->getName()
						)
			);
			continue;
		}
		switch (tmpChan->add(client, passwords[i].c_str()))
		{
		case Channel::SUCCESS:
			// send to client and all in the channel "<name> JOIN <channel>"
			tmpChan->send(client, "JOIN" + tmpChan->getName(), true, true);

			// Sends them RPL_TOPIC and RPL_TOPICTIME numerics if the channel
			// has a topic set (if the topic is not set, the user is sent no
			// numerics)
			if (!tmpChan->getTopic().empty())
			{
				sendToClient(
					client, RPL_TOPIC(
								client.second.getNickname(), tmpChan->getName(),
								tmpChan->getTopic()
							)
				);
				// TODO set who and when a topic is set
			}

			// Sends them one or more RPL_NAMREPLY numerics and a RPL_ENDOFNAMES
			//		(which also contain the name of the user that’s joining)
			sendToClient(
				client, RPL_NAMREPLY(
							client.second.getNickname(), tmpChan->getName(),
							usersName(*tmpChan)
						)
			);
			sendToClient(
				client,
				RPL_ENDOFNAMES(client.second.getNickname(), tmpChan->getName())
			);
			break;
		case Channel::USER_ALREADY:
			// TODO no idea
			break;
		case Channel::WRONG_PASSWORD:
			sendToClient(
				client, ERR_BADCHANNELKEY(
							client.second.getNickname(), tmpChan->getName()
						)
			);
			break;
		case Channel::CHANNELISFULL:
			sendToClient(
				client, ERR_CHANNELISFULL(
							client.second.getNickname(), tmpChan->getName()
						)
			);
			break;
		case Channel::INVITEONLYCHAN:
			sendToClient(
				client, ERR_INVITEONLYCHAN(
							client.second.getNickname(), tmpChan->getName()
						)
			);
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
		sendToClient(
			client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "part")
		);
		return 1;
	}
	std::vector<std::string> toLeave = splitChar(params.front(), ',');
	for (size_t i = 0; i != toLeave.size(); i++)
	{
		const ChannelsIt tmpChan = findChannel(channels, toLeave[i]);
		if (tmpChan != channels.end())
		{
			sendToClient(
				client, ERR_NOSUCHCHANNEL(
							client.second.getNickname(), tmpChan->getName()
						)
			);
			continue;
		}
		if (tmpChan->kick(client))
			tmpChan->send(
				client, "PART" + tmpChan->getName() + params[1], true, true
			);
		else
		{
			sendToClient(
				client, ERR_NOTONCHANNEL(
							client.second.getNickname(), tmpChan->getName()
						)
			);
		}
	}
	return 0;
}

int Exec::invite(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const FdClient& sender = clients.get(fd);
	const std::string& senderNick = sender.second.getNickname();

	const std::vector<std::string>& parameters = message.parameters();
	if (parameters.size() < 2)
	{
		sendToClient(sender, ERR_NEEDMOREPARAMS(senderNick, "INVITE"));
		return 1;
	}

	// Check if the channel exist
	const ChannelsIt channel = findChannel(channels, parameters[1]);
	if (channel == channels.end())
	{
		sendToClient(sender, ERR_NOSUCHCHANNEL(senderNick, parameters[1]));
		return 1;
	}

	if (!channel->isUser(sender))
	{
		sendToClient(sender, ERR_NOTONCHANNEL(senderNick, channel->getName()));
		return 1;
	}

	if (channel->inviteOnly && (!channel->isOperator(sender)))
	{
		sendToClient(
			sender, ERR_CHANOPRIVSNEEDED(senderNick, channel->getName())
		);
		return 1;
	}

	try
	{
		FdClient& target = clients.get(parameters[0].c_str());
		if (channel->isUser(target))
		{
			sendToClient(
				sender,
				ERR_USERONCHANNEL(
					senderNick, target.second.getNickname(), channel->getName()
				)
			);
			return 1;
		}

		sendToClient(
			sender,
			RPL_INVITING(
				senderNick, target.second.getNickname(), channel->getName()
			)
		);
		sendToClient(
			target, sender.second.getSource() + " INVITE " +
						target.second.getNickname() + " " + channel->getName()
		);
		channel->invite(target);
	}
	catch (std::invalid_argument& e)
	{
		sendToClient(sender, ERR_NOSUCHNICK(senderNick, parameters[0]));
		return 1;
	}

	return 0;
}

int Exec::privmsg(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const FdClient& client = clients.get(fd);
	const std::vector<std::string>& parameters = message.parameters();
	if (parameters.size() < 2)
	{
		sendToClient(
			client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "privmsg")
		);
		return 1;
	}

	std::vector<std::string> toSend = splitChar(parameters.front(), ',');
	// check if <text to be send> isEmpty -> send ERR_NOTEXTTOSEND "412 "
	if (parameters[1].empty())
	{
		sendToClient(client, ERR_NOTEXTTOSEND(client.second.getNickname()));
		return 1;
	}
	for (size_t i = 0; i != toSend.size(); i++)
	{
		// check if <target> isEmpty -> send ERR_NORECIPIENT "411 "
		if (toSend[i].empty())
		{
			sendToClient(client, ERR_NORECIPIENT(client.second.getNickname()));
			continue;
		}
		// check if it's channel (begin w #)
		if (toSend[i][0] == '#')
		{
			// if chan check if he is in the chan -> send in chan
			const ChannelsIt tmpChan = findChannel(channels, toSend[i]);
			if (tmpChan->isUser(client))
				tmpChan->send(client, parameters[1], true, true);
		}
		// if not chan check if it's a nickname -> send to nickname
		else
		{
			if (!clients.isNicknameUsed(toSend[i].c_str()))
			{
				sendToClient(
					client,
					ERR_NOSUCHNICK(client.second.getNickname(), toSend[i])
				);
				continue;
			}
			// TODO sendToClient();
		}
	}

	return 0;
}
