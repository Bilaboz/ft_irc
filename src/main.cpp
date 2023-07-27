/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 17:15:36 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/27 15:40:20 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include <csignal>
#include <iostream>
#include <stdexcept>

#include "Log.hpp"
#include "Server.hpp"

bool g_isRunning = true; // NOLINT

extern "C" void SIGINTHandler(int sig)
{
	(void)sig;

	g_isRunning = 0;
	const char message[] = "\nExiting...\n";
	// using write because we can't use any c++ stuff in a signal handler
	write(STDOUT_FILENO, message, sizeof(message));
}

int main(int argc, char** argv)
{
	if (argc < 2 || argc > 3)
	{
		std::cout << "Usage: ./ircserv PORT [PASSWORD]\n";
		return 1;
	}

	std::signal(SIGINT, SIGINTHandler);

	Log::setLevel(Log::DEBUG);

	try
	{
		Server server(argv[1], argv[2]);

		if (argv[2])
			Log::info() << "Server password: " << argv[2] << '\n';

		while (g_isRunning)
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
