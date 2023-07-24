/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 16:42:07 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/24 23:43:08 by rcarles          ###   ########.fr       */
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
	~Channel();

	Channel& operator=(const Channel& rhs);

	int kick(FdClient& user, std::vector<Channel>& channels);
	void invite(FdClient& user);
	int add(FdClient& user, const char* password);
	int promote(FdClient& user);
	int retrograde(FdClient& user);
	bool isOperator(const FdClient& user) const;
	bool isUser(const FdClient& user) const;
	bool isInvited(const FdClient& user) const;
	void setPassword(const char* password);
	void setUserLimit(int limit);
	std::string getName() const;
	std::vector<FdClient*> getUsers() const;
	std::string getTopic() const;
	void setTopic(const std::string& topic);
	std::string getModes() const;

	void send(
		const FdClient& author, const std::string& message, bool includeSource = false,
		bool sendToAuthor = false
	);

	bool inviteOnly;	   // NOLINT
	bool isTopicProtected; // NOLINT

	enum
	{
		SUCCESS = 0,
		USER_NOEXIST,
		USER_ALREADY,
		WRONG_PASSWORD,
		CHANNELISFULL,
		INVITEONLYCHAN
	};

  private:
	std::string m_name;
	std::string m_topic;
	std::vector<FdClient*> m_users;
	std::vector<FdClient*> m_operators;
	std::vector<FdClient*> m_invited;
	std::string m_password;
	int m_userLimit;
};

#endif
