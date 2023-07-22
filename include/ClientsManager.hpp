/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientsManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 21:51:44 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/22 20:56:49 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSMANAGER_HPP
#define CLIENTSMANAGER_HPP

#include <poll.h>

#include <map>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"

class ClientsManager
{
  public:
	ClientsManager();
	ClientsManager(const ClientsManager& other);
	ClientsManager& operator=(const ClientsManager& rhs);

	~ClientsManager();

	void addListener(int listener);
	void add(int fd);
	void remove(int fd, std::vector<Channel>& channels);
	void remove(const char* nickname, std::vector<Channel>& channels);
	FdClient& get(int fd);
	const FdClient& get(int fd) const;
	FdClient& get(const char* nickname);
	const FdClient& get(const char* nickname) const;
	std::vector<pollfd>& getPollfds();
	bool isNicknameUsed(const char* nick) const;

  private:
	std::vector<pollfd> m_fds;
	std::map<int, Client> m_clients;

	void addPollFd(int fd);
	void removePollFd(int fd);
};

#endif
