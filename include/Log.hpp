/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 16:39:26 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/07 19:20:08 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>

#define ORANGE "\e[1;33m"
#define BLUE "\e[1;34m"
#define GREY "\e[1;30m"
#define RED "\e[1;31m"
#define RESET "\e[0m"

#define DEBUG_HEADER "[*] " RESET
#define INFO_HEADER BLUE "[+] " RESET
#define WARNING_HEADER ORANGE "[WARNING] " RESET
#define ERROR_HEADER RED "[ERROR] " RESET

class Log
{
  public:
	enum
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR
	};

	static void setLevel(int level);
	static std::ostream &debug();
	static std::ostream &info();
	static std::ostream &warning();
	static std::ostream &error();

  private:
	static int m_level;
	static std::ofstream m_null;
};

#endif
