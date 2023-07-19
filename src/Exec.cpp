/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 15:20:32 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/19 15:52:04 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exec.hpp"

#include <map>
#include <utility>

const std::map<std::string, Exec::func> Exec::m_functions = Exec::initTable();

int Exec::exec(
	const Message &Message, ClientsManager &clients, int fd,
	std::vector<Channel> &channels
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
