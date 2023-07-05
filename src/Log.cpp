/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nthimoni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 16:47:44 by nthimoni          #+#    #+#             */
/*   Updated: 2023/07/05 19:04:27 by nthimoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>

#include "Log.hpp"

std::ofstream Log::m_null;
int Log::m_level = Log::DEBUG;

void Log::setLevel(int level)
{
	m_level = level;
}

std::ostream& Log::Debug()
{
	if (m_level <= DEBUG)
		return std::cout;

	return m_null;
}

std::ostream& Log::Info()
{
	if (m_level <= INFO)
		return std::cout;

	return m_null;
}

std::ostream& Log::Warning()
{
	if (m_level <= WARNING)
		return std::cout;

	return m_null;
}

std::ostream& Log::Error()
{
	if (m_level <= ERROR)
		return std::cerr;

	return m_null;
}
