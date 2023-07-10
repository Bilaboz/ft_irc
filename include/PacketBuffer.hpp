/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PacketBuffer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcarles <rcarles@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 18:42:15 by rcarles           #+#    #+#             */
/*   Updated: 2023/07/10 22:50:35 by rcarles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PACKETBUFFER_HPP
#define PACKETBUFFER_HPP

#define PACKET_BUFFER_SIZE 1024

#include <algorithm>
#include <cstring>
#include <string>

class PacketBuffer
{
  public:
	PacketBuffer();
	PacketBuffer(const PacketBuffer& other);
	~PacketBuffer();

	PacketBuffer& operator=(const PacketBuffer& rhs);

	bool getPacket(std::string& str);
	char const* getBuffer() const;
	bool isPacketReady() const;

  private:
	char m_buffer[PACKET_BUFFER_SIZE];
};

#endif
