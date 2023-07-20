/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 15:22:15 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/20 19:29:49 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <utility>
#include <vector>

#include "PacketBuffer.hpp"

class Channel;
class ClientsManager;

class Client
{
  public:
	Client();
	Client(const Client& other);
	Client& operator=(const Client& rhs);

	~Client();

	const std::string& getUsername() const;
	const std::string& getNickname() const;
	const std::string& getRealname() const;
	void setUsername(const char* username);
	void setNickname(const char* nickname);
	void setRealname(const char* realname);

	std::string receive(int fd, ClientsManager& clients);
	void reply(const std::string& message, bool includeSource = true) const;

	std::string getSource() const;

  private:
	std::string m_nickname;
	std::string m_username;
	std::string m_realname;
	PacketBuffer m_buffer;
};

typedef std::pair<const int, Client> FdClient;

#endif
