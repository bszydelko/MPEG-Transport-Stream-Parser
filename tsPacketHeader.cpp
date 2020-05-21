#include "tsPacketHeader.h"
#include "tsTransportStream.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

void xTS_PacketHeader::Reset()
{
	sync_byte.reset();
	transport_error_indicator.reset();
	payload_unit_start_indicator.reset();
	transport_priority.reset();
	PID.reset();
	transport_scrambling_control.reset();
	adaptation_field_control.reset();
	continuity_counter.reset();
}

int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
	std::istringstream H_bit_stream(xTS::getBitStream(Input, 0, xTS::TS_HeaderLength));

	H_bit_stream >> sync_byte;
	H_bit_stream >> transport_error_indicator;
	H_bit_stream >> payload_unit_start_indicator;
	H_bit_stream >> transport_priority;
	H_bit_stream >> PID;
	H_bit_stream >> transport_scrambling_control;
	H_bit_stream >> adaptation_field_control;
	H_bit_stream >> continuity_counter;

	//rownie dobrze reszte bitstreamu mozna zapisac i udostepniac pozostalym klasom

	return int32_t();
}

void xTS_PacketHeader::Print() const
{
	std::stringstream ss;

	ss <<
		"TS:"  <<
		" SB=" <<				  sync_byte.to_ulong()					  <<
		" E="  <<				  transport_error_indicator.to_ulong()    <<
		" S="  <<				  payload_unit_start_indicator.to_ulong() <<
		" P="  <<				  transport_priority.to_ulong()           <<
		" PID="<< std::setw(4) << PID.to_ulong()                          <<
		" TSC="<<				  transport_scrambling_control.to_ulong() <<
		" AFC="<<				  adaptation_field_control.to_ulong()     <<
		" CC=" << std::setw(2) << continuity_counter.to_ulong()           << " ";

	std::cout << ss.str();
}

uint8_t xTS_PacketHeader::getSyncByte() const
{
	return uint8_t(sync_byte.to_ulong());
}

uint8_t xTS_PacketHeader::getTransportErrorIndicator() const
{
	return uint8_t(transport_error_indicator.to_ulong());
}

uint8_t xTS_PacketHeader::getPayloadUnitStartIndicator() const
{
	return uint8_t(payload_unit_start_indicator.to_ulong());
}

uint8_t xTS_PacketHeader::getTransportPriority() const
{
	return uint8_t(transport_priority.to_ulong());
}

uint16_t xTS_PacketHeader::getPID() const
{
	return uint16_t(PID.to_ulong());
}

uint8_t xTS_PacketHeader::getTransportScramblingControl() const
{
	return uint8_t(transport_scrambling_control.to_ulong());
}

uint8_t xTS_PacketHeader::getAdaptationFieldControl() const
{
	return uint8_t(adaptation_field_control.to_ulong());
}
uint8_t xTS_PacketHeader::getContinuityCounter() const
{
	return uint8_t(continuity_counter.to_ulong());
}