/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcarles <rcarles@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 15:02:26 by rcarles           #+#    #+#             */
/*   Updated: 2023/07/13 16:57:45 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>

class Message
{
  public:
	Message();
	Message(const Message& other);
	Message(const std::string& msg);
	~Message();

	Message& operator=(const Message& rhs);

	void parse(const std::string& str);

	std::string verb() const;
	std::vector<std::string> parameters() const;

  private:
	std::string m_verb;
	std::vector<std::string> m_parameters;

	static void toUpper(std::string& str);
};

#endif
