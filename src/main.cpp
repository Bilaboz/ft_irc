/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcarles <rcarles@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 17:29:13 by rcarles           #+#    #+#             */
/*   Updated: 2023/07/07 18:00:45 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Log.hpp"

int main()
{
	Log::setLevel(Log::DEBUG);

	Log::debug() << "received packet from client\n";
	Log::info() << "new client connected\n";
	Log::warning() << "invalid fd passed to zebi()\n";
	Log::error() << "poll(): bla bla erreur grave\n";

	return 1;
}
