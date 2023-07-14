/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 15:22:15 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/13 16:18:26 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <utility>
#include <vector>

#include "PacketBuffer.hpp"

class Channel;

class Client
{
  public:
	Client();
	Client(const Client& other);
	Client& operator=(const Client& rhs);

	~Client();

	const std::string& getUsername() const;
	const std::string& getNickname() const;
	void setUsername(const char* username);
	void setNickanme(const char* nickname);
	void joinChannel(const char* name);
	void leaveChannel(const char* name);

  private:
	std::string m_nickname;
	std::string m_username;
	PacketBuffer buf;
	std::vector<Channel*> channels;
};

typedef std::pair<const int, Client> FdClient;

#endif
