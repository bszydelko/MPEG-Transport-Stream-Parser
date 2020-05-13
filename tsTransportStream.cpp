#include "tsTransportStream.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

std::string xTS::getBitStream(const uint8_t* _Input, size_t _startByte, size_t _count)
{
	std::string c_bin = "";
	std::string s_bin = "";
	int bit = 0;
	int c = 0;

	for (size_t byte = _startByte; byte < _count + _startByte; byte++)
	{
		c_bin = "";
		c = int(_Input[byte]);

		bit = 0;
		while (bit < CHAR_BIT)
		{
			(c % 2) ? c_bin.push_back('1') : c_bin.push_back('0');
			c >>= 1;
			bit++;
		}
		std::reverse(c_bin.begin(), c_bin.end());
		s_bin.append(c_bin);
	}

	return s_bin;
}

//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================

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
	//

	return int32_t();
}

void xTS_PacketHeader::Print() const
{
	std::stringstream ss;

	ss <<
		"TS:"   <<
		" SB="					  << sync_byte  .to_ulong() <<
		" E="					  << transport_error_indicator   .to_ulong() <<
		" S="					  << payload_unit_start_indicator   .to_ulong() <<
		" P="					  << transport_priority   .to_ulong() <<
		" PID=" << std::setw(4)   << PID .to_ulong() <<
		" TSC="					  << transport_scrambling_control .to_ulong() <<
		" AFC="					  << adaptation_field_control .to_ulong() <<
		" CC="  << std::setw(2)   << continuity_counter  .to_ulong();

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

uint32_t xTS_PacketHeader::getPID() const
{
	return uint32_t(PID.to_ulong());
}

uint8_t xTS_PacketHeader::getTransportScramblingControl() const
{
	return uint8_t(transport_scrambling_control.to_ulong());
}

uint16_t xTS_PacketHeader::getAdaptationFieldControl() const
{
	return uint16_t(adaptation_field_control.to_ulong());
}
uint8_t xTS_PacketHeader::getContinuityControl() const
{
	return uint8_t();
}
//=============================================================================================================================================================================




//=============================================================================================================================================================================
// xTS_AdaptationField
//=============================================================================================================================================================================
void xTS_AdaptationField::Reset()
{
	adaptation_field_length.reset();
	discontinuity_indicator.reset();
	random_access_indicator.reset();
	elementary_stream_priority_indicator.reset();
	PCR_flag.reset();
	OPCR_flag.reset();
	splicing_point_flag.reset();
	transport_private_data_flag.reset();
	adaptation_field_extension_flag.reset();

	program_clock_reference_base.reset();
	program_clock_reference_reserved.reset();
	program_clock_reference_extension.reset();
	
	original_program_clock_reference_base.reset();
	original_program_clock_reference_reserved.reset();
	original_program_clock_reference_extension.reset();

	splice_countdown.reset();

	transport_private_data_length.reset();

	adaptation_field_extension_length.reset();
	//delete transport_private_data;

	ltw_flag.reset();
	piecewise_rate_flag.reset();
	seamless_splice_flag.reset();
	adaptation_field_extension_reserved.reset();

	ltw_valid_flag.reset();
	ltw_offset.reset();

	piecewise_rate_reserved.reset();
	piecewise_rate.reset();
	
	splice_type.reset();
	DTS_next_access_unit.reset(); //do it later

	//optional_fields.reset();
}

int32_t xTS_AdaptationField::Parse(const uint8_t* Input, uint8_t AdapdationFieldControl)
{
	//dude, improve it
	int lengthAF = Input[xTS::TS_AdaptationFieldLengthByte];
	adaptation_field_length = lengthAF;


	lengthAF++;

	std::istringstream AF_bit_stream(xTS::getBitStream(Input, xTS::TS_HeaderLength + 1, lengthAF));

	AF_bit_stream >> discontinuity_indicator;
	AF_bit_stream >> random_access_indicator;
	AF_bit_stream >> elementary_stream_priority_indicator;
	AF_bit_stream >> PCR_flag;
	AF_bit_stream >> OPCR_flag;
	AF_bit_stream >> splicing_point_flag;
	AF_bit_stream >> transport_private_data_flag;
	AF_bit_stream >> adaptation_field_extension_flag;

	//potrzeba sprawdzenia na "bogatszym" w pakiet polach
	if (PCR_flag == 1)
	{
		AF_bit_stream >> program_clock_reference_base;
		AF_bit_stream >> program_clock_reference_reserved;
		AF_bit_stream >> program_clock_reference_extension;
	}
	if (OPCR_flag == 1)
	{
		AF_bit_stream >> original_program_clock_reference_base;
		AF_bit_stream >> original_program_clock_reference_reserved;
		AF_bit_stream >> original_program_clock_reference_extension;
	}
	if (splicing_point_flag == 1)
		AF_bit_stream >> splice_countdown;

	if (transport_private_data_flag == 1)
	{
		AF_bit_stream >> transport_private_data_length;
		transport_private_data = new uint8_t[transport_private_data_length.to_ulong()];
		AF_bit_stream >> transport_private_data;
	}
	if (adaptation_field_extension_flag == 1)
	{
		AF_bit_stream >> adaptation_field_extension_length;
		AF_bit_stream >> ltw_flag;
		AF_bit_stream >> piecewise_rate_flag;
		AF_bit_stream >> seamless_splice_flag;
		AF_bit_stream >> adaptation_field_extension_reserved;

		if (ltw_flag == 1)
		{
			AF_bit_stream >> ltw_valid_flag;
			AF_bit_stream >> ltw_offset;
		}
		if (piecewise_rate_flag == 1)
		{
			AF_bit_stream >> piecewise_rate_reserved;
			AF_bit_stream >> piecewise_rate;
		}
		if (seamless_splice_flag == 1)
		{
			AF_bit_stream >> splice_type;
			AF_bit_stream >> DTS_next_access_unit;
		}
		/*for (int i = 0; i < N; i++)
		{

		}*/

	}
	return int32_t();
}

void xTS_AdaptationField::Print() const
{
	std::stringstream ss;

	ss <<
		" AF:"  <<
		" L="   << std::setw(3) << adaptation_field_length.to_ulong() <<
		" DC="  << discontinuity_indicator  <<
		" RA="  << random_access_indicator  <<
		" SPI=" << elementary_stream_priority_indicator <<
		" PR="  << PCR_flag  <<
		" OR="  << OPCR_flag  <<
		" SPF=" << splicing_point_flag <<
		" TP="  << transport_private_data_flag  <<
		" EX="  << adaptation_field_extension_flag;

	std::cout << ss.str();

}






//=============================================================================================================================================================================
//    xPES_PacketHeader
//=============================================================================================================================================================================

void xPES_PacketHeader::Reset()
{
	m_PacketStartCodePrefix.reset();
	m_StreamId.reset();
	m_PacketLength.reset();
}

int32_t xPES_PacketHeader::Parse(const uint8_t* Input)
{
	std::istringstream PES_bit_stream(xTS::getBitStream(Input,))
	
	return int32_t();
}

void xPES_PacketHeader::Print() const
{
}


//=============================================================================================================================================================================
//    xPES_PacketHeader
//=============================================================================================================================================================================

void xPES_Assembler::Init(int32_t PID)
{
	m_LastContinuityCounter = -1;
	m_PID = PID;
}

eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField)
{
	if (PacketHeader->getPID() == m_PID && PacketHeader->hasPayload())
	{
		if (PacketHeader->getPayloadUnitStartIndicator() == 1)
		{
			if (PacketHeader->getContinuityControl() == 0) //parse header
			{
				m_Started = true;
				m_LastContinuityCounter = PacketHeader->getContinuityControl();
				m_PESH.Parse(TransportStreamPacket);
				
				return eResult::AssemblingStarted;
			}
		}
		else
		{
			if (PacketHeader->getContinuityControl() - m_LastContinuityCounter == 1)
			{
				m_LastContinuityCounter = PacketHeader->getContinuityControl();

				return eResult::AssemblingContinue;
			}
			else 
				return eResult::StreamPackedLost;

			//not sure if CC max is always 15
			if (PacketHeader->getContinuityControl() == 15)
			{
				m_Started = false;
				return eResult::AssemblingFinished;
			}
		}
	}

	return eResult();
}

void xPES_Assembler::xBufferReset()
{
}

void xPES_Assembler::xBufferAppend(const uint8_t* Data, int32_t Size)
{
}
