#include "tsTransportStream.h"
#include "pesPacketHeader.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

class xTS;
enum eStreamId;

void xPES_PacketHeader::Reset()
{
	m_Packet_start_code_prefix.reset();
	m_Stream_id.reset();
	m_PES_packet_length.reset();
}

int32_t xPES_PacketHeader::Parse(const uint8_t* Input, size_t start_byte)
{
	std::istringstream PES_bit_stream(xTS::getBitStream(Input, start_byte, xTS::PES_HeaderLength));
	

	PES_bit_stream >> m_Packet_start_code_prefix;
	PES_bit_stream >> m_Stream_id;
	PES_bit_stream >> m_PES_packet_length;


	if (m_Stream_id != eStreamId::eStreamId_program_stream_map
		&& m_Stream_id != eStreamId::eStreamId_padding_stream
		&& m_Stream_id != eStreamId::eStreamId_private_stream_2
		&& m_Stream_id != eStreamId::eStreamId_ECM
		&& m_Stream_id != eStreamId::eStreamId_EMM
		&& m_Stream_id != eStreamId::eStreamId_program_stream_directory
		&& m_Stream_id != eStreamId::eStreamId_DSMCC_stream
		&& m_Stream_id != eStreamId::eStreamId_ITUT_H222_1_type_E)
	{
		std::istringstream PES_bit_stream(xTS::getBitStream(Input, start_byte + xTS::PES_HeaderLength, 3));

		PES_bit_stream >> m_Marker_bits;
		PES_bit_stream >> m_PES_scrambling_control;
		PES_bit_stream >> m_PES_priority;
		PES_bit_stream >> m_Data_alignment_indicator;
		PES_bit_stream >> m_Copyright;
		PES_bit_stream >> m_Original_or_copy;

		PES_bit_stream >> m_PTS_DTS_flags;
		PES_bit_stream >> m_ESCR_flag;
		PES_bit_stream >> m_ES_rate_flag;
		PES_bit_stream >> m_DSM_trick_mode_flag;
		PES_bit_stream >> m_Additional_copy_info_flag;
		PES_bit_stream >> m_PES_CRC_flag;
		PES_bit_stream >> m_PES_extension_flag;
		PES_bit_stream >> m_PES_header_data_length;


		if (m_PTS_DTS_flags == 0b10)
		{
			PES_bit_stream >> m_PTS_data;
		}

		if (m_PTS_DTS_flags == 0b11)
		{
			PES_bit_stream >> m_PTS_data;
			PES_bit_stream >> m_DTS_data;
		}

		if (m_ESCR_flag == 0b1)
		{
			PES_bit_stream >> m_ESCR_data;
		}

		if (m_ES_rate_flag == 0b1)
		{
			PES_bit_stream >> m_ES_rate_data;
		}

		//rest of the fields blah blah blah
	}
	else if (m_Stream_id == eStreamId::eStreamId_program_stream_map
		|| m_Stream_id == eStreamId::eStreamId_private_stream_2
		|| m_Stream_id == eStreamId::eStreamId_ECM
		|| m_Stream_id == eStreamId::eStreamId_EMM
		|| m_Stream_id == eStreamId::eStreamId_program_stream_directory
		|| m_Stream_id == eStreamId::eStreamId_DSMCC_stream
		|| m_Stream_id == eStreamId::eStreamId_ITUT_H222_1_type_E)
	{
		//to powinno byc jako parsowanie calego pakietu a nie naglowka
		for (size_t i = 0; i < m_PES_packet_length.to_ulong(); i++)
		{

		}
	}
	else if (m_Stream_id == eStreamId::eStreamId_padding_stream)
	{
		//tu tak samo jak wyzej
		for (size_t i = 0; i < m_PES_packet_length.to_ulong(); i++)
		{

		}
	}
	
	return int32_t();
}

void xPES_PacketHeader::Print() const
{
	std::stringstream ss;

	ss << "PES:"
		<< " PSCP=" << m_Packet_start_code_prefix.to_ulong()
		<< " SID=" << m_Stream_id.to_ulong()
		<< " L=" << m_PES_packet_length.to_ulong();

	if (m_PTS_DTS_flags == 0b10)
	{
		//print pts
		
		ss << " PTS=" << "(Time=" << "s)";
	}

	if (m_PTS_DTS_flags == 0b11)
	{
		//print pts, dts
		ss << " PTS=" << "(Time=" << "s)" << " DTS=" << "(Time=" << "s)";
	}

	std::cout << ss.str();
}