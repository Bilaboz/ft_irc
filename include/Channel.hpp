/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 16:42:07 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/13 16:48:04 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include "Client.hpp"

class Channel
{
  public:
	Channel();
	Channel(const char* name);
	Channel(const Channel& other);

	Channel& operator=(const Channel& rhs);

	~Channel();

	int kick(FdClient& user);
	int add(FdClient& user, const char* password);
	int promote(FdClient& user);
	int retrograde(FdClient& user);
	void msg(char* msg, const FdClient& sender);
	bool isOperator(const FdClient& user);
	bool isUser(const FdClient& user);
	bool isInvited(const FdClient& user);
	bool setPassword(const char *password);
	void setUserLimit(int limit);

	bool inviteOnly;
	bool topicOperator;

  private:
	std::string m_name;
	std::vector<FdClient*> m_users;
	std::vector<FdClient*> m_operators;
	std::vector<FdClient*> m_invited;
	std::string m_password;
	int m_userLimit;
};

#endif
