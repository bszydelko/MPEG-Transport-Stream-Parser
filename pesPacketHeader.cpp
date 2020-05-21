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

int32_t xPES_PacketHeader::Parse(const uint8_t* Input)
{
	std::stringstream PES_bit_stream(xTS::getBitStream(Input, 0, xTS::PES_HeaderLength));
	
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
		PES_bit_stream.str("");  
		PES_bit_stream.clear(); //header length is not known, read 3 bytes for flags
		PES_bit_stream << xTS::getBitStream(Input, xTS::PES_HeaderLength, 3);

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

		PES_bit_stream.str("");  
		PES_bit_stream.clear();												// header length is known
		PES_bit_stream << xTS::getBitStream(Input, xTS::PES_HeaderLength + 3, m_PES_header_data_length.to_ulong());


		if (m_PTS_DTS_flags == 0b10)
		{
			PES_bit_stream >> m_PTS_junk; //'0010'
			PES_bit_stream >> m_PTS_32_30;
			PES_bit_stream >> m_PTS_marker;
			PES_bit_stream >> m_PTS_29_15;
			PES_bit_stream >> m_PTS_marker;
			PES_bit_stream >> m_PTS_14_0;
			PES_bit_stream >> m_PTS_marker;
		
		}

		if (m_PTS_DTS_flags == 0b11)
		{
			PES_bit_stream >> m_PTS_junk; //'0010'
			PES_bit_stream >> m_PTS_32_30;
			PES_bit_stream >> m_PTS_marker;
			PES_bit_stream >> m_PTS_29_15;
			PES_bit_stream >> m_PTS_marker;
			PES_bit_stream >> m_PTS_14_0;
			PES_bit_stream >> m_PTS_marker;

			PES_bit_stream >> m_DTS_junk; //'0001'
			PES_bit_stream >> m_DTS_32_30;
			PES_bit_stream >> m_DTS_marker;
			PES_bit_stream >> m_DTS_29_15;
			PES_bit_stream >> m_DTS_marker;
			PES_bit_stream >> m_DTS_14_0;
			PES_bit_stream >> m_DTS_marker;
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
		ss << " PTS=" << "(Time=" << "s)";
	}

	if (m_PTS_DTS_flags == 0b11)
	{
		ss << " PTS=" << "(Time=" << "s)" << " DTS=" << "(Time=" << "s)";
	}

	std::cout << ss.str();
}