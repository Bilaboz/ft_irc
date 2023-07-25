/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcarles <rcarles@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 17:54:41 by rcarles           #+#    #+#             */
/*   Updated: 2023/07/25 19:07:05 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPL_HPP
#define RPL_HPP

#define RPL_WELCOME(client, source)                                                      \
	("001 " + (client) + " :Welcome to the ircserv Network, " + (source))

#define RPL_UMODEIS(client, modes) ("221 " + (client) + " :" + (modes))

#define RPL_WHOISUSER(client, nick, username, host, realname)                            \
	("311 " + (client) + " " + (nick) + " " + (username) + " " + (host) +                \
	 " * :" + (realname))

#define RPL_WHOISERVER(client, nick, server, serverinfo)                                 \
	("312 " + (client) + " " + (nick) + " " + (server) + " :" + (serverinfo))

#define RPL_ENDOFWHO(client, mask)                                                       \
	("315 " + (client) + " " + (mask) + " :End of WHO list")

#define RPL_ENDOFWHOIS(client, nick)                                                     \
	("318 " + (client) + " " + (nick) + " :End of /WHOIS list")

#define RPL_CHANNELMODEIS(client, channel, mode)                                         \
	("324 " + (client) + " " + (channel) + " :" + (mode))

#define RPL_NOTOPIC(client, channel)                                                     \
	("331 " + (client) + " " + (channel) + " :No topic is set")

#define RPL_TOPIC(client, channel, topic)                                                \
	("332 " + (client) + " " + (channel) + " :" + (topic))

#define RPL_TOPICWHOTIME(client, channel, nick, setat)                                   \
	("333 " + (client) + " " + (channel) + " " + (nick) + " " + (setat))

#define RPL_INVITING(client, nick, channel)                                              \
	("341 " + (client) + " " + (nick) + " " + (channel))

#define RPL_WHOREPLY(                                                                    \
	client, channel, username, host, server, nick, flags, hopcount, realname             \
)                                                                                        \
	("352 " + (client) + " " + (channel) + " " + (username) + " " + (host) +             \
	 " " server + " " + (nick) + " " + (flags) + " :" + (hopcount) + " " + (realname))

#define RPL_NAMREPLY(client, channel, users)                                             \
	("353 " + (client) + " = " + (channel) + " " + (users))

#define RPL_ENDOFNAMES(client, channel)                                                  \
	("366 " + (client) + " " + (channel) + " :End of /NAMES list")

#define ERR_NOSUCHNICK(client, nick) ("401 " + (client) + " " + (nick) + " :No such nick")

#define ERR_NOSUCHCHANNEL(client, channel)                                               \
	("403 " + (client) + " " + (channel) + " :No such channel")

#define ERR_NORECIPIENT(client) ("411 " + (client) + " :No recipient given (PRIVMSG)")

#define ERR_NOTEXTTOSEND(client) ("412 " + (client) + " :No text to send")

#define ERR_NONICKNAMEGIVEN(client) ("431 " + (client) + " :No nickname given")

#define ERR_ERRONEUSNICKNAME(client, nick)                                               \
	("432 " + (client) + " " + (nick) + " :Erroneus nickname")

#define ERR_NICKNAMEINUSE(client, nick)                                                  \
	("433 " + (client) + " " + (nick) + " :Nickname is already in use")

#define ERR_USERNOTINCHANNEL(client, nick, channel)                                      \
	("441 " + (client) + " " + (nick) + " " + (channel) + " :User(s) not on channel")

#define ERR_NOTONCHANNEL(client, channel)                                                \
	("442 " + (client) + " " + (channel) + " :You're not on that channel")

#define ERR_USERONCHANNEL(client, nick, channel)                                         \
	("443 " + (client) + " " + (nick) + " " + (channel) + " :is already on channel")

#define ERR_NEEDMOREPARAMS(client, command)                                              \
	("461 " + (client) + " " command " :Not enough parameters")

#define ERR_ALREADYREGISTERED(client) ("462 " + (client) + " :You may not reregister")

#define ERR_PASSWDMISMATCH(client) ("464 " + (client) + " :Incorrect password")

#define ERR_CHANNELISFULL(client, channel)                                               \
	("471 " + (client) + " " + (channel) + " :Cannot join " + (channel) + " (+l)")

#define ERR_INVITEONLYCHAN(client, channel)                                              \
	("473 " + (client) + " " + (channel) + " :Cannot join " + (channel) + " (+i)")

#define ERR_BADCHANNELKEY(client, channel)                                               \
	("475 " + (client) + " " + (channel) + " :Cannot join " + (channel) + " (+k)")

#define ERR_CHANOPRIVSNEEDED(client, channel)                                            \
	("482 " + (client) + " " + (channel) + " :You're not " + (channel) + " operator")

#define ERR_USERSDONTMATCH(client)                                                       \
	("502 " + (client) + " :Can't change mode for other users")

#endif
