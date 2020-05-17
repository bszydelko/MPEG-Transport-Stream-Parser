#include "tsTransportStream.h"
#include "pesPacketHeader.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

class xTS;

void xPES_PacketHeader::Reset()
{
	m_PacketStartCodePrefix.reset();
	m_StreamId.reset();
	m_PacketLength.reset();
}

int32_t xPES_PacketHeader::Parse(const uint8_t* Input, size_t start_byte)
{
	std::istringstream PES_bit_stream(xTS::getBitStream(Input, start_byte, 6));

	PES_bit_stream >> m_PacketStartCodePrefix;
	PES_bit_stream >> m_StreamId;
	PES_bit_stream >> m_PacketLength;

	return int32_t();
}

void xPES_PacketHeader::Print() const
{
	std::stringstream ss;

	ss << "PES:"
		<< " PSCP=" << m_PacketStartCodePrefix.to_ulong()
		<< " SID=" << m_StreamId.to_ulong()
		<< " L=" << m_PacketLength.to_ulong();

	std::cout << ss.str();
}