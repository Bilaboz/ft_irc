/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PacketBuffer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcarles <rcarles@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 18:46:33 by rcarles           #+#    #+#             */
/*   Updated: 2023/07/28 18:32:09 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PacketBuffer.hpp"

#include "Log.hpp"

PacketBuffer::~PacketBuffer() {}

PacketBuffer::PacketBuffer() : m_size() {}

PacketBuffer::PacketBuffer(const PacketBuffer& other)
{
	*this = other;
}

PacketBuffer& PacketBuffer::operator=(const PacketBuffer& rhs)
{
	if (this != &rhs)
	{
		std::copy(rhs.m_buffer, rhs.m_buffer + rhs.m_size, m_buffer);
		m_size = rhs.m_size;
	}

	return *this;
}

bool PacketBuffer::isPacketReady() const
{
	const char* end = m_buffer + m_size;

	const char* found = std::find(m_buffer, end, '\r');
	return (found != end && *(++found) == '\n');
}

int PacketBuffer::receive(int fd)
{
	const ssize_t received = recv(fd, m_buffer + m_size, PACKET_BUFFER_SIZE - m_size, 0);
	if (received == -1)
	{
		Log::error() << "recv(): " << std::strerror(errno) << '\n';
		return RECV_ERROR;
	}

	if (received == 0)
		return CLIENT_DISCONNECTED;

	m_size += received;
	return isPacketReady();
}

std::string PacketBuffer::getPacket()
{
	if (!isPacketReady())
		return "";

	char* end = m_buffer + m_size;
	const char* found = std::find(m_buffer, end, '\r');
	const size_t packetSize = found - m_buffer;

	const std::string str(m_buffer, packetSize);

	std::copy(m_buffer + packetSize + 2, end, m_buffer);
	m_size -= packetSize + 2;

	return str;
}
