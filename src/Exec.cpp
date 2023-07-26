/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 15:20:32 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/27 00:54:55 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exec.hpp"

#include <sys/socket.h>

#include <climits>
#include <cstring>
#include <map>
#include <stdexcept>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"
#include "Log.hpp"
#include "RPL.hpp"
#include "Server.hpp"

const std::map<std::string, Exec::func> Exec::m_functions = Exec::initTable();
const char* Exec::m_serverPassword = NULL; // NOLINT

int Exec::exec(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels, const char* password
)
{
	const FdClient& client = clients.get(fd);
	std::string verb = message.verb();

	Exec::m_serverPassword = password;
	if (password != NULL && verb != "CAP" && verb != "PASS" &&
		!client.second.hasSentPassword)
	{
		sendToClient(client, "ERROR :Invalid password");
		clients.remove(fd, channels);
		return -2;
	}

	if (!client.second.isRegistered && verb != "CAP" && verb != "NICK" &&
		verb != "USER" && verb != "PASS")
	{
		sendToClient(client, "ERROR :You must register");
		clients.remove(fd, channels);
		return -2;
	}

	const std::map<std::string, func>::const_iterator func = m_functions.find(verb);

	if (func != m_functions.end())
		return func->second(message, clients, fd, channels);

	Log::warning() << "Unknown command: [" << message.verb() << "]\n";

	return -1;
}

std::map<std::string, Exec::func> Exec::initTable()
{
	std::map<std::string, func> ret;

	ret.insert(std::make_pair("WHO", &Exec::who));
	ret.insert(std::make_pair("PING", &Exec::ping));
	ret.insert(std::make_pair("QUIT", &Exec::quit));
	ret.insert(std::make_pair("JOIN", &Exec::join));
	ret.insert(std::make_pair("NICK", &Exec::nick));
	ret.insert(std::make_pair("PASS", &Exec::pass));
	ret.insert(std::make_pair("PART", &Exec::part));
	ret.insert(std::make_pair("KICK", &Exec::kick));
	ret.insert(std::make_pair("MODE", &Exec::mode));
	ret.insert(std::make_pair("USER", &Exec::user));
	ret.insert(std::make_pair("TOPIC", &Exec::topic));
	ret.insert(std::make_pair("WHOIS", &Exec::whois));
	ret.insert(std::make_pair("INVITE", &Exec::invite));
	ret.insert(std::make_pair("NOTICE", &Exec::privmsg));
	ret.insert(std::make_pair("PRIVMSG", &Exec::privmsg));

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
		const ssize_t sent = send(client.first, data.c_str() + bytesSent, data.size(), 0);
		if (sent == -1)
		{
			Log::error() << "send(): " << std::strerror(errno) << '\n';
			return;
		}

		bytesSent += sent;
	}

	Log::debug() << "Sent (fd=" << client.first << "): " << message << '\n';
}

int Exec::topic(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const FdClient& sender = clients.get(fd);
	const std::string& senderNick = sender.second.getNickname();
	if (message.parameters().empty())
	{
		sendToClient(sender, ERR_NEEDMOREPARAMS(senderNick, "TOPIC"));
		return 1;
	}

	const ChannelsIt channelIt = findChannel(channels, message.parameters().front());
	if (channelIt == channels.end())
	{
		sendToClient(sender, ERR_NOSUCHCHANNEL(senderNick, message.parameters()[0]));
		return 1;
	}

	const std::string& chanTopic = channelIt->getTopic();
	if (message.parameters().size() == 1)
	{
		if (chanTopic.empty())
			sendToClient(sender, RPL_NOTOPIC(senderNick, channelIt->getName()));
		else
		{
			sendToClient(sender, RPL_TOPIC(senderNick, channelIt->getName(), chanTopic));
			sendToClient(
				sender,
				RPL_TOPICWHOTIME(
					senderNick,
					channelIt->getName(),
					channelIt->getTopicSetter(),
					channelIt->getTopicTimestamp()
				)
			);
		}
		return 0;
	}

	if (channelIt->isTopicProtected && !channelIt->isOperator(sender))
	{
		sendToClient(sender, ERR_CHANOPRIVSNEEDED(senderNick, channelIt->getName()));
		return 1;
	}

	channelIt->setTopic(message.parameters()[1], senderNick);

	channelIt->send(
		sender, "TOPIC " + channelIt->getName() + " :" + channelIt->getTopic(), true, true
	);

	return 0;
}

int Exec::user(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	(void)channels;
	FdClient& sender = clients.get(fd);
	const std::vector<std::string>& parameters = message.parameters();
	const std::string& senderNick = sender.second.getNickname();

	if (parameters.size() < 4 || parameters[3].empty())
	{
		sendToClient(sender, ERR_NEEDMOREPARAMS(senderNick, "USER"));
		return 1;
	}

	if (sender.second.isRegistered)
	{
		sendToClient(sender, ERR_ALREADYREGISTERED(senderNick));
		return 1;
	}

	sender.second.setUsername(parameters[0].c_str());
	sender.second.setRealname(parameters[3].c_str());

	if (!senderNick.empty() && !sender.second.isRegistered)
	{
		sender.second.isRegistered = true;
		sendToClient(sender, RPL_WELCOME(senderNick, sender.second.getSource()));
		sendToClient(sender, RPL_YOURHOST(senderNick));
		sendToClient(sender, RPL_CREATED(senderNick, Server::startDate));
		sendToClient(sender, RPL_MYINFO(senderNick));

		std::vector<std::string> motd = splitChar(MOTD, '\n');
		for (size_t i = 0; i < motd.size(); ++i)
			sendToClient(sender, RPL_MOTD(senderNick, motd[i]));
	}

	return 0;
}

bool Exec::isNicknameValid(const std::string& str)
{
	if (str[0] == ':' || str[0] == '#' || str[0] == '$')
		return false;

	if (str.find_first_of(" ,*?!@.") != std::string::npos)
		return false;

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
		sendToClient(client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "nick"));
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
		sendToClient(client, ERR_ERRONEUSNICKNAME(client.second.getNickname(), nickname));
		return 1;
	}

	if (clients.isNicknameUsed(nickname.c_str()))
	{
		std::string clientNick = client.second.getNickname();
		if (clientNick.empty())
			clientNick = "*";
		sendToClient(client, ERR_NICKNAMEINUSE(clientNick, nickname));
		return 1;
	}

	client.second.setNickname(nickname.c_str());

	if (!client.second.getUsername().empty() && !client.second.isRegistered)
	{
		client.second.isRegistered = true;
		sendToClient(client, RPL_WELCOME(nickname, client.second.getSource()));
		sendToClient(client, RPL_YOURHOST(client.second.getNickname()));
		sendToClient(client, RPL_CREATED(client.second.getNickname(), Server::startDate));
		sendToClient(client, RPL_MYINFO(client.second.getNickname()));

		std::vector<std::string> motd = splitChar(MOTD, '\n');
		for (size_t i = 0; i < motd.size(); ++i)
			sendToClient(client, RPL_MOTD(client.second.getNickname(), motd[i]));
	}

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
		sendToClient(client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "kick"));
		return 1;
	}

	std::vector<std::string> toKickFrom = splitChar(params.front(), ',');
	std::vector<std::string> passwords = splitChar(params[1], ',');
	ChannelsIt tmpChan;

	for (std::vector<std::string>::iterator channelIt = toKickFrom.begin();
		 channelIt != toKickFrom.end();
		 channelIt++)
	{
		tmpChan = findChannel(channels, *channelIt);
		if (tmpChan == channels.end())
		{
			sendToClient(
				client, ERR_NOSUCHCHANNEL(client.second.getNickname(), tmpChan->getName())
			);
			continue;
		}

		for (std::vector<std::string>::iterator nickIt = passwords.begin();
			 nickIt != passwords.end();
			 nickIt++)
		{
			if (!tmpChan->isUser(client))
			{
				sendToClient(
					client,
					ERR_NOTONCHANNEL(client.second.getNickname(), tmpChan->getName())
				);
				break;
			}

			if (!tmpChan->isOperator(client))
			{
				sendToClient(
					client,
					ERR_CHANOPRIVSNEEDED(client.second.getNickname(), tmpChan->getName())
				);
				break;
			}

			if (!clients.isNicknameUsed(nickIt->c_str()) ||
				!tmpChan->isUser(clients.get((*nickIt).c_str())))
			{
				sendToClient(
					client,
					ERR_USERNOTINCHANNEL(
						client.second.getNickname(), *nickIt, tmpChan->getName()
					)
				);
				continue;
			}

			if (params[2].empty())
				tmpChan->send(
					client, "KICK " + tmpChan->getName() + " " + *nickIt, true, true
				);
			else
			{
				tmpChan->send(
					client,
					"KICK " + tmpChan->getName() + " " + *nickIt + " :" + params[2],
					true,
					true
				);
			}

			tmpChan->kick(clients.get((*nickIt).c_str()), channels);
		}
	}

	return 0;
}

std::string Exec::usersName(Channel& chan)
{
	std::vector<FdClient*> vec = chan.getUsers();
	std::string ret = ":";
	if (chan.isOperator(*vec[0]))
		ret += "@";
	ret += vec[0]->second.getNickname();

	for (size_t i = 1; i < vec.size(); i++)
	{
		ret += " ";

		if (chan.isOperator(*vec[i]))
			ret += "@";

		ret += vec[i]->second.getNickname();
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

	if (params.empty())
	{
		sendToClient(client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "join"));
		return 1;
	}

	std::vector<std::string> toJoin = splitChar(params.front(), ',');
	std::vector<std::string> passwords;

	if (params.size() > 1)
		passwords = splitChar(params[1], ',');

	while (passwords.size() < toJoin.size())
		passwords.push_back("");

	for (size_t i = 0; i != toJoin.size(); i++)
	{
		ChannelsIt tmpChan = findChannel(channels, toJoin[i]);
		bool channelCreated = false;

		if (tmpChan == channels.end())
		{
			Channel newChannel(toJoin[i].c_str());

			newChannel.setPassword(passwords[i].c_str());
			channels.push_back(newChannel);
			tmpChan = channels.end() - 1;
			channelCreated = true;
		}

		switch (tmpChan->add(client, passwords[i].c_str())) // NOLINT
		{
		case Channel::SUCCESS:
			// send in the channel that someone has joined
			tmpChan->send(client, "JOIN " + tmpChan->getName(), true, true);

			if (!tmpChan->getTopic().empty())
			{
				sendToClient(
					client,
					RPL_TOPIC(
						client.second.getNickname(),
						tmpChan->getName(),
						tmpChan->getTopic()
					)
				);
				sendToClient(
					client,
					RPL_TOPICWHOTIME(
						client.second.getNickname(),
						tmpChan->getName(),
						tmpChan->getTopicSetter(),
						tmpChan->getTopicTimestamp()
					)
				);
				// TODO send RPL_TOPICWHOTIME
			}

			// give op privilege to the first user on the channel
			if (channelCreated)
				tmpChan->promote(client);

			sendToClient(
				client,
				RPL_NAMREPLY(
					client.second.getNickname(), tmpChan->getName(), usersName(*tmpChan)
				)
			);
			sendToClient(
				client, RPL_ENDOFNAMES(client.second.getNickname(), tmpChan->getName())
			);
			break;

		case Channel::WRONG_PASSWORD:
			sendToClient(
				client, ERR_BADCHANNELKEY(client.second.getNickname(), tmpChan->getName())
			);
			break;

		case Channel::CHANNELISFULL:
			sendToClient(
				client, ERR_CHANNELISFULL(client.second.getNickname(), tmpChan->getName())
			);
			break;

		case Channel::INVITEONLYCHAN:
			sendToClient(
				client,
				ERR_INVITEONLYCHAN(client.second.getNickname(), tmpChan->getName())
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
		sendToClient(client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "part"));
		return 1;
	}

	std::vector<std::string> toLeave = splitChar(params.front(), ',');

	for (size_t i = 0; i != toLeave.size(); i++)
	{
		const ChannelsIt tmpChan = findChannel(channels, toLeave[i]);
		if (tmpChan == channels.end())
		{
			sendToClient(
				client, ERR_NOSUCHCHANNEL(client.second.getNickname(), toLeave[i])
			);
			continue;
		}

		if (tmpChan->isUser(client))
		{
			std::string answer = "PART " + tmpChan->getName();

			if (params.size() > 1)
				answer += " " + params[1];

			tmpChan->send(client, answer, true, true);
			tmpChan->kick(client, channels);
		}
		else
		{
			sendToClient(
				client, ERR_NOTONCHANNEL(client.second.getNickname(), tmpChan->getName())
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
		sendToClient(sender, ERR_CHANOPRIVSNEEDED(senderNick, channel->getName()));
		return 1;
	}

	if (!clients.isNicknameUsed(parameters[0].c_str()))
	{
		sendToClient(sender, ERR_NOSUCHNICK(senderNick, parameters[0]));
		return 1;
	}

	FdClient& target = clients.get(parameters[0].c_str());
	if (channel->isUser(target))
	{
		sendToClient(
			sender,
			ERR_USERONCHANNEL(senderNick, target.second.getNickname(), channel->getName())
		);
		return 1;
	}

	sendToClient(
		sender, RPL_INVITING(senderNick, target.second.getNickname(), channel->getName())
	);

	const std::string response = sender.second.getSource() + " INVITE " +
								 target.second.getNickname() + " " + channel->getName();
	sendToClient(target, response);
	channel->invite(target);

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
			client, ERR_NEEDMOREPARAMS(client.second.getNickname(), message.verb())
		);
		return 1;
	}

	std::vector<std::string> toSend = splitChar(parameters.front(), ',');

	// check if <text to be sent> isEmpty -> ERR_NOTEXTTOSEND
	if (parameters[1].empty())
	{
		sendToClient(client, ERR_NOTEXTTOSEND(client.second.getNickname()));
		return 1;
	}

	for (size_t i = 0; i != toSend.size(); i++)
	{
		// check if <target> isEmpty -> ERR_NORECIPIENT
		if (toSend[i].empty())
		{
			sendToClient(client, ERR_NORECIPIENT(client.second.getNickname()));
			continue;
		}

		// if it starts with # -> it's a channel
		if (toSend[i][0] == '#')
		{
			const ChannelsIt tmpChan = findChannel(channels, toSend[i]);
			if (tmpChan == channels.end())
				sendToClient(
					client, ERR_NOSUCHCHANNEL(client.second.getNickname(), toSend[i])
				);
			else if (tmpChan->isUser(client))
			{
				tmpChan->send(
					client,
					message.verb() + " " + toSend[i] + " :" + parameters[1],
					true,
					false
				);
			}
		}
		else // send to user directly (not in a channel)
		{
			if (!clients.isNicknameUsed(toSend[i].c_str()))
			{
				sendToClient(
					client, ERR_NOSUCHNICK(client.second.getNickname(), toSend[i])
				);
				continue;
			}
			const FdClient& target = clients.get(toSend[i].c_str());
			sendToClient(
				target,
				client.second.getSource() + " " + message.verb() + " " + toSend[i] +
					" :" + parameters[1]
			);
		}
	}

	return 0;
}

int Exec::ping(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	(void)channels;
	std::string token;

	if (!message.parameters().empty())
		token = message.parameters().front();

	sendToClient(clients.get(fd), "PONG " + token);

	return 0;
}

int Exec::quit(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const FdClient& sender = clients.get(fd);
	std::string reason;

	if (!message.parameters().empty())
		reason = message.parameters().front();

	for (size_t i = 0; i < channels.size(); ++i)
	{
		if (channels[i].isUser(sender))
			channels[i].send(sender, "QUIT :Quit: " + reason, true);
	}

	clients.remove(fd, channels);

	return -2;
}

int Exec::whois(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	(void)channels;
	const FdClient& sender = clients.get(fd);
	const std::string senderNick = sender.second.getNickname();

	if (message.parameters().empty())
	{
		sendToClient(sender, ERR_NEEDMOREPARAMS(sender.second.getNickname(), "WHOIS"));
		return 1;
	}

	const std::string targetNick = message.parameters().front();
	if (targetNick.empty())
	{
		sendToClient(sender, ERR_NONICKNAMEGIVEN(senderNick));
		return 1;
	}

	if (!clients.isNicknameUsed(targetNick.c_str()))
	{
		sendToClient(sender, ERR_NOSUCHNICK(senderNick, message.parameters().front()));
		return 1;
	}

	const Client& target = clients.get(message.parameters().front().c_str()).second;

	sendToClient(
		sender,
		RPL_WHOISUSER(
			senderNick, targetNick, target.getUsername(), "*", target.getRealname()
		)
	);

	sendToClient(
		sender,
		RPL_WHOISERVER(
			senderNick,
			targetNick,
			std::string("ircserv"),
			std::string("ircsrv by rcarles, lbesnard, nthimoni")
		)
	);

	sendToClient(sender, RPL_ENDOFWHOIS(senderNick, targetNick));

	return 0;
}

int Exec::mode(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const FdClient& client = clients.get(fd);
	const std::vector<std::string>& params = message.parameters();

	if (params.empty())
	{
		sendToClient(client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "MODE"));
		return 1;
	}

	// if target doesn't start with # -> it's an user, we do not handle it
	if (params.front()[0] != '#')
	{
		sendToClient(client, RPL_UMODEIS(client.second.getNickname(), ""));
		return 0;
	}

	const ChannelsIt channel = findChannel(channels, params.front());
	if (channel == channels.end())
	{
		sendToClient(
			client, ERR_NOSUCHCHANNEL(client.second.getNickname(), params.front())
		);
		return 1;
	}

	if (params.size() == 1)
	{
		sendToClient(
			client,
			RPL_CHANNELMODEIS(
				client.second.getNickname(), params.front(), channel->getModes()
			)
		);
		return 0;
	}

	const std::string& modeStr = params[1];
	if (modeStr.find_first_not_of("+-itkol") != std::string::npos ||
		(modeStr[0] != '+' && modeStr[0] != '-') || modeStr.size() == 1)
		return 1;

	if (!channel->isOperator(client))
	{
		sendToClient(
			client, ERR_CHANOPRIVSNEEDED(client.second.getNickname(), params.front())
		);
		return 1;
	}

	bool isMinus = false;
	unsigned int paramsIdx = 2;
	std::string appliedModes;
	std::stringstream appliedModesParameters;
	for (size_t i = 0; i < modeStr.size(); ++i)
	{
		if (modeStr[i] == '-' || modeStr[i] == '+')
		{
			isMinus = (modeStr[i] == '-');
			appliedModes += isMinus ? '-' : '+';
			continue;
		}

		switch (modeStr[i]) // NOLINT
		{
		case 'i':
			channel->inviteOnly = !isMinus;
			appliedModes += 'i';
			break;

		case 't':
			channel->isTopicProtected = !isMinus;
			appliedModes += 't';
			break;

		case 'k':
			if (isMinus)
			{
				channel->setPassword("");
				appliedModes += 'k';
				break;
			}

			if (params.size() <= paramsIdx)
			{
				Log::warning() << "Not enough parameters for mode " << modeStr[i] << '\n';
				break;
			}

			channel->setPassword(params[paramsIdx].c_str());
			appliedModes += 'k';
			appliedModesParameters << ' ' << params[paramsIdx];
			paramsIdx++;
			break;

		case 'o':
			if (params.size() <= paramsIdx)
			{
				Log::warning() << "Not enough parameters for mode " << modeStr[i] << '\n';
				break;
			}

			if (!clients.isNicknameUsed(params[paramsIdx].c_str()))
				break;

			{
				FdClient& target = clients.get(params[paramsIdx].c_str());
				if (!channel->isUser(target))
					break;

				isMinus ? channel->retrograde(target) : channel->promote(target);
			}

			appliedModes += 'o';
			appliedModesParameters << ' ' << params[paramsIdx];
			paramsIdx++;
			break;

		case 'l':
			if (isMinus)
				channel->setUserLimit(0);

			if (params.size() <= paramsIdx)
			{
				Log::warning() << "Not enough parameters for mode " << modeStr[i] << '\n';
				break;
			}

			size_t limit = std::strtoul(params[paramsIdx].c_str(), NULL, 10); // NOLINT
			// TODO: CHECK FOR (LIMIT > 0)

			int intLimit = 0;
			if (limit > INT_MAX)
				intLimit = INT_MAX;
			else
				intLimit = limit; // NOLINT

			channel->setUserLimit(intLimit);
			appliedModes += 'l';
			appliedModesParameters << ' ' << intLimit;
			paramsIdx++;
			break;
		}
	}

	// special case where we had one mode specified but not enough parameters were
	// given / there was an invalid parameter and we're left with "-" or "+" in
	// appliedModes
	if (appliedModes.size() == 1)
		return 0;

	const std::string answer =
		"MODE " + params.front() + " " + appliedModes + appliedModesParameters.str();

	channel->send(client, answer, true, true);

	return 0;
}

int Exec::who(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	const FdClient& sender = clients.get(fd);
	const std::string& senderNick = sender.second.getNickname();

	if (message.parameters().empty() || message.parameters().front().empty())
	{
		sendToClient(sender, ERR_NEEDMOREPARAMS(senderNick, "WHO"));
		return 1;
	}

	const std::string& target = message.parameters().front();
	// The target is a channel
	if (target[0] == '#')
	{
		const Exec::ChannelsIt chan = findChannel(channels, target);
		if (chan != channels.end())
		{
			std::vector<FdClient*> users = chan->getUsers();
			for (std::vector<FdClient*>::iterator it = users.begin(); it != users.end();
				 ++it)
			{
				const FdClient& userPair = **it;
				const Client& user = (*it)->second;
				std::string flags = "H";
				if (chan->isOperator(userPair))
					flags += "@";
				sendToClient(
					sender,
					RPL_WHOREPLY(
						senderNick,
						target,
						user.getUsername(),
						user.getHost(),
						"ft_irc",
						user.getNickname(),
						flags,
						"0",
						user.getRealname()
					)
				);
			}
		}
	}
	// The target is a user
	else if (clients.isNicknameUsed(target.c_str()))
	{
		const FdClient& userPair = clients.get(target.c_str());
		const Client& user = userPair.second;

		sendToClient(
			sender,
			RPL_WHOREPLY(
				senderNick,
				"*",
				user.getUsername(),
				"host",
				"ircserv",
				user.getNickname(),
				"H",
				"0",
				user.getRealname()
			)
		);
	}

	sendToClient(sender, RPL_ENDOFWHO(senderNick, target));

	return 0;
}

int Exec::pass(
	const Message& message, ClientsManager& clients, int fd,
	std::vector<Channel>& channels
)
{
	(void)channels;
	FdClient& client = clients.get(fd);

	if (Exec::m_serverPassword == NULL)
		return 0;

	if (message.parameters().empty())
	{
		sendToClient(client, ERR_NEEDMOREPARAMS(client.second.getNickname(), "PASS"));
		return 1;
	}

	if (Exec::m_serverPassword != message.parameters().front())
	{
		sendToClient(client, ERR_PASSWDMISMATCH(client.second.getNickname()));
		sendToClient(client, client.second.getSource() + " ERROR :Invalid password");
		return 1;
	}

	if (client.second.hasSentPassword || client.second.isRegistered)
	{
		sendToClient(client, ERR_ALREADYREGISTERED(client.second.getNickname()));
		return 1;
	}

	client.second.hasSentPassword = true;
	return 0;
}
