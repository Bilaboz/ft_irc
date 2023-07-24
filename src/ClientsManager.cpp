/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientsManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 21:53:35 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/24 23:43:24 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientsManager.hpp"

#include <sys/poll.h>
#include <unistd.h>

#include <stdexcept>
#include <utility>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"
#include "Log.hpp"

ClientsManager::ClientsManager() {}

ClientsManager::ClientsManager(const ClientsManager& other)
	: m_fds(other.m_fds), m_clients(other.m_clients)
{
}

ClientsManager& ClientsManager::operator=(const ClientsManager& rhs)
{
	if (this != &rhs)
	{
		m_fds = rhs.m_fds;
		m_clients = rhs.m_clients;
	}

	return *this;
}

ClientsManager::~ClientsManager()
{
	for (std::vector<pollfd>::const_iterator it = m_fds.begin(); it != m_fds.end(); it++)
		close(it->fd);
}

void ClientsManager::addListener(int listener)
{
	this->addPollFd(listener);
}

void ClientsManager::add(int fd)
{
	this->addPollFd(fd);
	m_clients.insert(std::make_pair(fd, Client()));
}

void ClientsManager::remove(int fd, std::vector<Channel>& channels)
{
	const std::map<int, Client>::iterator clientIt = m_clients.find(fd);

	if (clientIt == m_clients.end())
	{
		throw std::invalid_argument(
			"ClientsManager::remove(int fd): fd isn't bound to any Client"
		);
	}

	Log::info() << "Client on fd " << fd << " (" << clientIt->second.getNickname()
				<< ") disconnected\n";

	for (size_t i = 0; i < channels.size(); ++i)
		channels[i].kick(*clientIt, channels);
	close(clientIt->first);
	this->removePollFd(clientIt->first);
	m_clients.erase(clientIt);
}

void ClientsManager::remove(const char* nickname, std::vector<Channel>& channels)
{
	for (std::map<int, Client>::iterator clientIt = m_clients.begin();
		 clientIt != m_clients.end();
		 ++clientIt)
	{
		if (clientIt->second.getNickname() == nickname)
		{
			for (std::vector<Channel>::iterator it = channels.begin();
				 it != channels.end();
				 ++it)
				it->kick(*clientIt, channels);
			close(clientIt->first);
			this->removePollFd(clientIt->first);
			m_clients.erase(clientIt);

			Log::info() << "Client on fd " << clientIt->first << " ("
						<< clientIt->second.getNickname() << ") disconnected\n";
			return;
		}
	}

	std::string exception("ClientsManager::remove(const char* nickname): no client named "
	);
	exception += nickname;

	throw std::invalid_argument(exception);
}

FdClient& ClientsManager::get(int fd)
{
	const std::map<int, Client>::iterator clientIt = m_clients.find(fd);

	if (clientIt == m_clients.end())
		throw std::invalid_argument(
			"ClientsManager::get(int fd): fd isn't bound to any Client"
		);

	return *clientIt;
}

const FdClient& ClientsManager::get(int fd) const
{
	const std::map<int, Client>::const_iterator clientIt = m_clients.find(fd);

	if (clientIt == m_clients.end())
		throw std::invalid_argument(
			"ClientsManager::get(int fd): fd isn't bound to any Client"
		);

	return *clientIt;
}

FdClient& ClientsManager::get(const char* nickname)
{
	for (std::map<int, Client>::iterator it = m_clients.begin(); it != m_clients.end();
		 ++it)
	{
		if (it->second.getNickname() == nickname)
			return *it;
	}

	std::string exception("ClientsManager::get(const char* nickname): no client named ");
	exception += nickname;

	throw std::invalid_argument(exception);
}

const FdClient& ClientsManager::get(const char* nickname) const
{
	for (std::map<int, Client>::const_iterator it = m_clients.begin();
		 it != m_clients.end();
		 ++it)
	{
		if (it->second.getNickname() == nickname)
			return *it;
	}

	std::string exception("ClientsManager::get(const char* nickname): no client named ");
	exception += nickname;

	throw std::invalid_argument(exception);
}

std::vector<pollfd>& ClientsManager::getPollfds()
{
	return m_fds;
}

bool ClientsManager::isNicknameUsed(const char* nick) const
{
	for (std::map<int, Client>::const_iterator it = m_clients.begin();
		 it != m_clients.end();
		 it++)
		if (it->second.getNickname() == nick)
			return true;
	return false;
}

void ClientsManager::addPollFd(int fd)
{
	pollfd newElem;
	newElem.fd = fd;
	newElem.events = POLLIN;
	newElem.revents = 0;
	m_fds.push_back(newElem);
}

void ClientsManager::removePollFd(int fd)
{
	for (std::vector<pollfd>::iterator it = m_fds.begin(); it != m_fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			m_fds.erase(it);
			return;
		}
	}
}
