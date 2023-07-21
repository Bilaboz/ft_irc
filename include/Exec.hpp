/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 14:30:38 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/21 00:12:38 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_HPP
#define EXEC_HPP

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "ClientsManager.hpp"
#include "Message.hpp"

class Exec
{
  public:
	typedef int (*func)(const Message&, ClientsManager&, int, std::vector<Channel>&);
	typedef std::vector<Channel>::iterator ChannelsIt;

	static int exec(
		const Message& message, ClientsManager& clients, int fd,
		std::vector<Channel>& channels
	);

	static void sendToClient(
		const FdClient& client, const std::string& message,
		bool includeSource = true
	);

  private:
	static const std::map<std::string, func> m_functions;

	static std::map<std::string, func> initTable();
	static bool isNicknameValid(const std::string& str);
	static std::vector<std::string> splitChar(
		const std::string& str, char c = ','
	);

	static ChannelsIt findChannel(
		std::vector<Channel>& channels, const std::string& name
	);

	static int
	join(const Message& message, ClientsManager& clients, int fd, std::vector<Channel>&);
	static int
	nick(const Message& message, ClientsManager& clients, int fd, std::vector<Channel>&);
	static int
	kick(const Message& message, ClientsManager& clients, int fd, std::vector<Channel>&);
	static int
	topic(const Message& message, ClientsManager& clients, int fd, std::vector<Channel>&);
	static int
	user(const Message& message, ClientsManager& clients, int fd, std::vector<Channel>&);
	static int
	part(const Message& message, ClientsManager& clients, int fd, std::vector<Channel>&);
	static int
	privmsg(const Message& message, ClientsManager& clients, int fd, std::vector<Channel>&);
};

#endif
