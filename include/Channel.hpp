/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 16:42:07 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/12 17:01:05 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP 

#include "Client.hpp"

class Channel
{
	public:
		Channel();
		Channel(const Channel& other);

		Channel& operator=(const Channel& rhs);

		~Channel();

		void kick(Client& user);
		void add(Client& user);
		void promote(Client& user);
		void retrograde(Client& user);
		void msg(char *msg);
		bool isOperator(Client& user);
		bool isUser(Client& user);

	private:
		std::string m_name;
		std::vector<Client*> m_users;
		std::vector<Client*> m_operators;
};


#endif 
