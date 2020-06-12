#include "tsPacketHeader.h"
#include "tsTransportStream.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

void xTS_PacketHeader::Reset()
{
	m_Sync_byte.reset();
	m_Transport_error_indicator.reset();
	m_Payload_unit_start_indicator.reset();
	m_Transport_priority.reset();
	m_PID.reset();
	m_Transport_scrambling_control.reset();
	m_Adaptation_field_control.reset();
	m_Continuity_counter.reset();
}

int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
	std::istringstream H_bit_stream(xTS::getBitStream(Input, 0, xTS::TS_HeaderLength));

	H_bit_stream >> m_Sync_byte;
	H_bit_stream >> m_Transport_error_indicator;
	H_bit_stream >> m_Payload_unit_start_indicator;
	H_bit_stream >> m_Transport_priority;
	H_bit_stream >> m_PID;
	H_bit_stream >> m_Transport_scrambling_control;
	H_bit_stream >> m_Adaptation_field_control;
	H_bit_stream >> m_Continuity_counter;

	//rownie dobrze reszte bitstreamu mozna zapisac i udostepniac pozostalym klasom

	return int32_t();
}

void xTS_PacketHeader::Print() const
{
	std::stringstream ss;

	ss <<
		"TS:"  <<
		" SB=" <<				  m_Sync_byte.to_ulong()					  <<
		" E="  <<				  m_Transport_error_indicator.to_ulong()    <<
		" S="  <<				  m_Payload_unit_start_indicator.to_ulong() <<
		" P="  <<				  m_Transport_priority.to_ulong()           <<
		" PID="<< std::setw(4) << m_PID.to_ulong()                          <<
		" TSC="<<				  m_Transport_scrambling_control.to_ulong() <<
		" AFC="<<				  m_Adaptation_field_control.to_ulong()     <<
		" CC=" << std::setw(2) << m_Continuity_counter.to_ulong()           << " ";

	std::cout << ss.str();
}

uint8_t xTS_PacketHeader::getSyncByte() const
{
	return uint8_t(m_Sync_byte.to_ulong());
}

uint8_t xTS_PacketHeader::getTransportErrorIndicator() const
{
	return uint8_t(m_Transport_error_indicator.to_ulong());
}

uint8_t xTS_PacketHeader::getPayloadUnitStartIndicator() const
{
	return uint8_t(m_Payload_unit_start_indicator.to_ulong());
}

uint8_t xTS_PacketHeader::getTransportPriority() const
{
	return uint8_t(m_Transport_priority.to_ulong());
}

uint16_t xTS_PacketHeader::getPID() const
{
	return uint16_t(m_PID.to_ulong());
}

uint8_t xTS_PacketHeader::getTransportScramblingControl() const
{
	return uint8_t(m_Transport_scrambling_control.to_ulong());
}

uint8_t xTS_PacketHeader::getAdaptationFieldControl() const
{
	return uint8_t(m_Adaptation_field_control.to_ulong());
}
uint8_t xTS_PacketHeader::getContinuityCounter() const
{
	return uint8_t(m_Continuity_counter.to_ulong());
}