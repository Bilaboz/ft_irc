/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PacketBuffer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcarles <rcarles@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 18:42:15 by rcarles           #+#    #+#             */
/*   Updated: 2023/07/20 15:59:10 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PACKETBUFFER_HPP
#define PACKETBUFFER_HPP

#define PACKET_BUFFER_SIZE 1024
#define PACKET_SIZE 512

#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <string>

class PacketBuffer
{
  public:
	PacketBuffer();
	PacketBuffer(const PacketBuffer& other);
	~PacketBuffer();

	PacketBuffer& operator=(const PacketBuffer& rhs);

	int receive(int fd);
	std::string getPacket();
	bool isPacketReady() const;

	enum
	{
		PACKET_NOT_READY = 0,
		PACKET_READY,
		CLIENT_DISCONNECTED,
		RECV_ERROR
	};

  private:
	char m_buffer[PACKET_BUFFER_SIZE];
	size_t m_size;
};

#endif
