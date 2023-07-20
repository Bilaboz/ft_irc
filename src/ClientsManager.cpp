/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientsManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 21:53:35 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/19 18:39:57 by lbesnard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientsManager.hpp"

#include <sys/poll.h>
#include <unistd.h>

#include <stdexcept>
#include <utility>
#include <vector>

#include "Client.hpp"

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
	for (std::vector<pollfd>::const_iterator it = m_fds.begin();
		 it != m_fds.end(); it++)
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

void ClientsManager::remove(int fd)
{
	std::map<int, Client>::iterator ret = m_clients.find(fd);

	if (ret == m_clients.end())
		throw std::invalid_argument(
			"ClientsManager::remove(int fd): fd isn't bound to any Client"
		);
	close(ret->first);
	m_clients.erase(ret);
}

void ClientsManager::remove(const char* nickname)
{
	for (std::map<int, Client>::iterator it = m_clients.begin();
		 it != m_clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
		{
			close(it->first);
			m_clients.erase(it);
			return;
		}
	}

	std::string exception(
		"ClientsManager::remove(const char* nickname): no client named "
	);
	exception += nickname;

	throw std::invalid_argument(exception);
}

FdClient& ClientsManager::get(int fd)
{
	std::map<int, Client>::iterator ret = m_clients.find(fd);

	if (ret == m_clients.end())
		throw std::invalid_argument(
			"ClientsManager::get(int fd): fd isn't bound to any Client"
		);

	return *ret;
}

const FdClient& ClientsManager::get(int fd) const
{
	std::map<int, Client>::const_iterator ret = m_clients.find(fd);

	if (ret == m_clients.end())
		throw std::invalid_argument(
			"ClientsManager::get(int fd): fd isn't bound to any Client"
		);

	return *ret;
}

FdClient& ClientsManager::get(const char* nickname)
{
	for (std::map<int, Client>::iterator it = m_clients.begin();
		 it != m_clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return *it;
	}

	std::string exception(
		"ClientsManager::get(const char* nickname): no client named "
	);
	exception += nickname;

	throw std::invalid_argument(exception);
}

const FdClient& ClientsManager::get(const char* nickname) const
{
	for (std::map<int, Client>::const_iterator it = m_clients.begin();
		 it != m_clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return *it;
	}

	std::string exception(
		"ClientsManager::get(const char* nickname): no client named "
	);
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
		 it != m_clients.end(); it++)
		if (it->second.getNickname() == nick)
			return true;
	return false;
}

void ClientsManager::addPollFd(int fd)
{
	pollfd newElem;
	newElem.fd = fd;
	newElem.events = POLLIN;
	m_fds.push_back(newElem);
}
