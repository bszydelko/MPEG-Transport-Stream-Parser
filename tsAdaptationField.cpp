#include "tsAdaptationField.h"
#include "tsTransportStream.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

class xTS;

xTS_AdaptationField::xTS_AdaptationField()
{
	m_Transport_private_data = nullptr;
	m_Stuffing_bytes_length = 0;
	m_Stuffing_bytes = nullptr;
}

void xTS_AdaptationField::Reset()
{
	m_Adaptation_field_length.reset();
	m_Discontinuity_indicator.reset();
	m_Random_access_indicator.reset();
	m_Elementary_stream_priority_indicator.reset();
	m_PCR_flag.reset();
	m_OPCR_flag.reset();
	m_Splicing_point_flag.reset();
	m_Transport_private_data_flag.reset();
	m_Adaptation_field_extension_flag.reset();

	m_Program_clock_reference_base.reset();
	m_Program_clock_reference_reserved.reset();
	m_Program_clock_reference_extension.reset();

	m_Original_program_clock_reference_base.reset();
	m_Original_program_clock_reference_reserved.reset();
	m_Original_program_clock_reference_extension.reset();

	m_Splice_countdown.reset();

	m_Transport_private_data_length.reset();

	m_Adaptation_field_extension_length.reset();
	//delete transport_private_data;

	m_Ltw_flag.reset();
	m_Piecewise_rate_flag.reset();
	m_Seamless_splice_flag.reset();
	m_Adaptation_field_extension_reserved.reset();

	m_Ltw_valid_flag.reset();
	m_Ltw_offset.reset();

	m_Piecewise_rate_reserved.reset();
	m_Piecewise_rate.reset();

	m_Splice_type.reset();
	m_DTS_next_access_unit.reset(); //do it later

	//optional_fields.reset();
	/*if (stuffing_bytes != nullptr)
		delete[] stuffing_bytes;*/

}

int32_t xTS_AdaptationField::Parse(const uint8_t* Input, uint8_t AdapdationFieldControl)
{
	m_Adaptation_field_length = Input[xTS::TS_AdaptationFieldLengthByte];
	m_Stuffing_bytes_length = m_Adaptation_field_length.to_ulong() - 1; //substracted in IF statements

	std::istringstream AF_bit_stream(xTS::getBitStream(Input, xTS::TS_AdaptationFieldLengthByte + 1, m_Adaptation_field_length.to_ulong()));

	AF_bit_stream >> m_Discontinuity_indicator;
	AF_bit_stream >> m_Random_access_indicator;
	AF_bit_stream >> m_Elementary_stream_priority_indicator;
	AF_bit_stream >> m_PCR_flag;
	AF_bit_stream >> m_OPCR_flag;
	AF_bit_stream >> m_Splicing_point_flag;
	AF_bit_stream >> m_Transport_private_data_flag;
	AF_bit_stream >> m_Adaptation_field_extension_flag;

	//potrzeba sprawdzenia na "bogatszym" w pola pakiet
	if (m_PCR_flag == 0b1)
	{
		AF_bit_stream >> m_Program_clock_reference_base;
		AF_bit_stream >> m_Program_clock_reference_reserved;
		AF_bit_stream >> m_Program_clock_reference_extension;

		m_Stuffing_bytes_length -= 6;
	}
	if (m_OPCR_flag == 0b1)
	{
		AF_bit_stream >> m_Original_program_clock_reference_base;
		AF_bit_stream >> m_Original_program_clock_reference_reserved;
		AF_bit_stream >> m_Original_program_clock_reference_extension;

		m_Stuffing_bytes_length -= 6;
	}
	if (m_Splicing_point_flag == 0b1)
	{
		AF_bit_stream >> m_Splice_countdown;

		m_Stuffing_bytes_length -= 1;
	}

	if (m_Transport_private_data_flag == 0b1)
	{
		AF_bit_stream >> m_Transport_private_data_length;
		m_Transport_private_data = new uint8_t[m_Transport_private_data_length.to_ulong()];
		AF_bit_stream >> m_Transport_private_data;

		m_Stuffing_bytes_length -= m_Transport_private_data_length.to_ulong();
	}
	if (m_Adaptation_field_extension_flag == 0b1)
	{
		AF_bit_stream >> m_Adaptation_field_extension_length;
		AF_bit_stream >> m_Ltw_flag;
		AF_bit_stream >> m_Piecewise_rate_flag;
		AF_bit_stream >> m_Seamless_splice_flag;
		AF_bit_stream >> m_Adaptation_field_extension_reserved;

		m_Stuffing_bytes_length -= m_Adaptation_field_extension_length.to_ulong() - 1;

		if (m_Ltw_flag == 0b1)
		{
			AF_bit_stream >> m_Ltw_valid_flag;
			AF_bit_stream >> m_Ltw_offset;

			m_Stuffing_bytes_length -= 2;
		}
		if (m_Piecewise_rate_flag == 0b1)
		{
			AF_bit_stream >> m_Piecewise_rate_reserved;
			AF_bit_stream >> m_Piecewise_rate;

			m_Stuffing_bytes_length -= 3;
		}
		if (m_Seamless_splice_flag == 0b1)
		{
			AF_bit_stream >> m_Splice_type;
			AF_bit_stream >> m_DTS_next_access_unit;

			m_Stuffing_bytes_length -= 5;
		}

	}
	/*if (stuffing_bytes_length > 0)
	{
		stuffing_bytes = new uint8_t[stuffing_bytes_length];
		for (size_t byte = 0; byte < stuffing_bytes_length; byte++)
		{
			AF_bit_stream >> stuffing_bytes[byte];
		}
	}*/
	return int32_t();
}

void xTS_AdaptationField::Print() const
{
	std::stringstream ss;

	ss <<
		"AF:"  <<
		" L="  << std::setw(3) <<	m_Adaptation_field_length.to_ulong()	<<
		" DC=" <<					m_Discontinuity_indicator				<<
		" RA=" <<					m_Random_access_indicator				<<
		" SPI="<<					m_Elementary_stream_priority_indicator<<
		" PR=" <<					m_PCR_flag							<<
		" OR=" <<					m_OPCR_flag							<<
		" SPF="<<					m_Splicing_point_flag					<<
		" TP=" <<					m_Transport_private_data_flag			<<
		" EX=" <<					m_Adaptation_field_extension_flag;
	
	if (m_PCR_flag == 0b1)
	{
		uint32_t PCR = m_Program_clock_reference_base.to_ulong() * xTS::BaseToExtendedClockMultiplier + m_Program_clock_reference_extension.to_ulong();
		double time = (double)PCR / (double)xTS::ExtendedClockFrequency_Hz; //wtf it works XD
		ss << " PCR=" << PCR << " (Time=" << std::setprecision(7) << time << "s)";
	}

	if (m_OPCR_flag == 0b1)
	{
		uint32_t OPCR = m_Original_program_clock_reference_base.to_ulong() * xTS::BaseToExtendedClockMultiplier + m_Original_program_clock_reference_extension.to_ulong();
		double time = (double)OPCR / (double)xTS::ExtendedClockFrequency_Hz; 
		ss << " OPCR=" << OPCR << " (Time=" << std::setprecision(7) << time << "s)";
	}
	ss <<" Stuffing=" << m_Stuffing_bytes_length << " ";

	std::cout << ss.str();
}

uint32_t xTS_AdaptationField::getNumBytes() const
{
	return uint32_t();
}

uint8_t xTS_AdaptationField::getAdaptationFieldLength() const
{
	return uint8_t(m_Adaptation_field_length.to_ulong());
}
