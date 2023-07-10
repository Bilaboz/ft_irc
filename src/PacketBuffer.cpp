/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PacketBuffer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcarles <rcarles@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 18:46:33 by rcarles           #+#    #+#             */
/*   Updated: 2023/07/10 22:50:53 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PacketBuffer.hpp"

PacketBuffer::~PacketBuffer() {}

PacketBuffer::PacketBuffer()
{
	std::memset(m_buffer, 0, PACKET_BUFFER_SIZE);
}

PacketBuffer::PacketBuffer(const PacketBuffer& other)
{
	*this = other;
}

PacketBuffer& PacketBuffer::operator=(const PacketBuffer& rhs)
{
	if (this != &rhs)
		std::copy(rhs.m_buffer, rhs.m_buffer + PACKET_BUFFER_SIZE, m_buffer);

	return *this;
}

bool PacketBuffer::isPacketReady() const
{
	const char* end = m_buffer + PACKET_BUFFER_SIZE;

	const char* found = std::find(m_buffer, end, '\r');
	return (found != end && *(++found) == '\n');
}

char const* PacketBuffer::getBuffer() const
{
	return m_buffer + std::strlen(m_buffer);
}

bool PacketBuffer::getPacket(std::string& str)
{
	if (!isPacketReady())
		return false;

	char* end = m_buffer + PACKET_BUFFER_SIZE;
	const char* found = std::find(m_buffer, end, '\r');
	size_t packetSize = found - m_buffer;

	str.append(m_buffer, packetSize);

	char* endOfCopy = std::copy(m_buffer + packetSize + 2, end, m_buffer);
	std::memset(endOfCopy, 0, end - endOfCopy);

	return true;
}
