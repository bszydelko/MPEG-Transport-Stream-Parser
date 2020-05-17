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

int32_t xPES_PacketHeader::Parse(const uint8_t* Input)
{
	//std::istringstream PES_bit_stream(xTS::getBitStream(Input, ))

		return int32_t();
}

void xPES_PacketHeader::Print() const
{
}