/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 17:15:36 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/20 18:28:12 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>

#include "Log.hpp"
#include "Server.hpp"

int main(int argc, char** argv)
{
	if (argc < 2 || argc > 3)
	{
		std::cout << "Usage: ./ircserv PORT [PASSWORD]\n";
		return 1;
	}

	try
	{
		Server server(argv[1]);

		while (1)
		{
			server.poll();
		}
	}
	catch (std::runtime_error& e)
	{
		Log::error() << e.what() << '\n';
	}

	return 0;
}
