/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 16:39:26 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/04 20:48:13 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
# define LOG_HPP 

#include <iostream>

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
		static std::ostream& Debug();
		static std::ostream& Info();
		static std::ostream& Warning();
		static std::ostream& Error();

	private:
		static int m_level;
		static std::ofstream m_null;
};

#endif 
