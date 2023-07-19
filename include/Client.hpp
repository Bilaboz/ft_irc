/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 15:22:15 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/19 20:46:06 by nthimoni         ###   ########.fr       */
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
	const std::string& getRealname() const;
	void setUsername(const char* username);
	void setNickname(const char* nickname);
	void setRealname(const char* realname);

  private:
	std::string m_nickname;
	std::string m_username;
	std::string m_realname;
	PacketBuffer buf;
};

typedef std::pair<const int, Client> FdClient;

#endif
