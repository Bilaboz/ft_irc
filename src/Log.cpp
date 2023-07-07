/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 16:47:44 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/07 19:19:41 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Log.hpp"

#include <fstream>
#include <iostream>

std::ofstream Log::m_null;
int Log::m_level = Log::DEBUG;

void Log::setLevel(int level)
{
	m_level = level;
}

std::ostream &Log::debug()
{
	if (m_level > DEBUG)
		return m_null;

	std::cout << DEBUG_HEADER;
	return std::cout;
}

std::ostream &Log::info()
{
	if (m_level > INFO)
		return m_null;

	std::cout << INFO_HEADER;
	return std::cout;
}

std::ostream &Log::warning()
{
	if (m_level > WARNING)
		return m_null;

	std::cout << WARNING_HEADER;
	return std::cout;
}

std::ostream &Log::error()
{
	if (m_level > ERROR)
		return m_null;

	std::cerr << ERROR_HEADER;
	return std::cerr;
}
