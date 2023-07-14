/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientsManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 21:51:44 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/14 14:30:15 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSMANAGER_HPP
#define CLIENTSMANAGER_HPP

#include <poll.h>

#include <map>
#include <vector>

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
	void remove(int fd);
	void remove(const char* nickname);
	FdClient& get(int fd);
	const FdClient& get(int fd) const;
	FdClient& get(const char* nickname);
	const FdClient& get(const char* nickname) const;
	const std::vector<pollfd>& getPollfds() const;

  private:
	std::vector<pollfd> m_fds;
	std::map<int, Client> m_clients;

	void addPollFd(int fd);
};

#endif
