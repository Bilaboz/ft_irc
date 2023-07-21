/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcarles <rcarles@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 17:54:41 by rcarles           #+#    #+#             */
/*   Updated: 2023/07/21 19:48:08 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPL_HPP
#define RPL_HPP

#define RPL_WELCOME(client, source)                                            \
	("001 " + client + " :Welcome to the ircserv Network, "  + source)
#define RPL_NOTOPIC(client, channel)                                           \
	("331 " + client + " " + channel + " :No topic is set")
#define RPL_TOPIC(client, channel, topic) ("332 " + client + " " + channel + " :"  + topic)
#define RPL_TOPICWHOTIME(client, channel, nick, setat)                         \
	("333 " + client + " " + channel + " " + nick + " " + setat)
#define RPL_INVITING(client, nick, channel) ("341 " + client + " " + nick + " " channel)
#define ERR_NOSUCHNICK(client, nick) ("401 " + client + " " + nick +  " :No such nick")
#define ERR_NOSUCHCHANNEL(client, channel)                                     \
	("403 " + client + " " + channel + " :No such channel")
#define ERR_NONICKNAMEGIVEN(client) ("431 " + client + " :No nickname given")
#define ERR_ERRONEUSNICKNAME(client, nick)                                     \
	("432 " + client + " " + nick +  " :Erroneus nickname")
#define ERR_NICKNAMEINUSE(client, nick)                                        \
	("433 " + client + " " + nick + " :Nickname is already in use")
#define ERR_USERNOTINCHANNEL(client, nick, channel)                            \
	("441 " + client + " " + nick + " " + channel + " :User(s) not on channel")
#define ERR_NOTONCHANNEL(client, channel)                                      \
	("442 " + client + " " + channel + " :You're not on that channel")
#define ERR_USERONCHANNEL(client, nick, channel)                               \
	("443 " + client + " " + nick + " " + channel + " :is already on channel")
#define ERR_NEEDMOREPARAMS(client, command)                                    \
	("461 " + client + " " command " :Not enough parameters")
#define ERR_ALREADYREGISTERED(client) ("462 " + client + " :You may not reregister")
#define ERR_CHANNELISFULL(client, channel)                                     \
	("471 " + client + " " + channel + " :Cannot join + channel + (+l)")
#define ERR_INVITEONLYCHAN(client, channel)                                    \
	("473 " + client + " " + channel + " :Cannot join + channel + (+i)")
#define ERR_BADCHANNELKEY(client, channel)                                     \
	("475 " + client + " " + channel + " :Cannot join + channel + (+k)")
#define ERR_CHANOPRIVSNEEDED                                                   \
	("482 " + client + " " + channel + " :You're not + channel + operator")

#endif
