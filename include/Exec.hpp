/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 14:30:38 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/19 15:52:26 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_HPP
#define EXEC_HPP

#include <map>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "ClientsManager.hpp"
#include "Message.hpp"

class Exec
{
  public:
	static int exec(
		const Message& Message, ClientsManager& clients, int fd,
		std::vector<Channel>& channels
	);

	typedef int (*func)(const Message&, ClientsManager&, int, std::vector<Channel>&);

  private:
	static std::map<std::string, func> initTable();

	static const std::map<std::string, func> m_functions;

	static int
	join(const Message& Message, ClientsManager& clients, int fd, std::vector<Channel>&);
	static int
	nick(const Message& Message, ClientsManager& clients, int fd, std::vector<Channel>&);
	static int
	kick(const Message& Message, ClientsManager& clients, int fd, std::vector<Channel>&);
};

#endif
