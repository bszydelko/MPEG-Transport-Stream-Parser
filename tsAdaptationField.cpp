#include "tsAdaptationField.h"
#include "tsTransportStream.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

class xTS;

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
	stuffing_bytes_length = adaptation_field_length.to_ulong() - 1;

	//lengthAF++;
	

	std::istringstream AF_bit_stream(xTS::getBitStream(Input, xTS::TS_AdaptationFieldLengthByte + 1, lengthAF));

	AF_bit_stream >> discontinuity_indicator;
	AF_bit_stream >> random_access_indicator;
	AF_bit_stream >> elementary_stream_priority_indicator;
	AF_bit_stream >> PCR_flag;
	AF_bit_stream >> OPCR_flag;
	AF_bit_stream >> splicing_point_flag;
	AF_bit_stream >> transport_private_data_flag;
	AF_bit_stream >> adaptation_field_extension_flag;

	//potrzeba sprawdzenia na "bogatszym" w pola pakiet
	if (PCR_flag == 1)
	{
		AF_bit_stream >> program_clock_reference_base;
		AF_bit_stream >> program_clock_reference_reserved;
		AF_bit_stream >> program_clock_reference_extension;

		stuffing_bytes_length -= 6;
	}
	if (OPCR_flag == 1)
	{
		AF_bit_stream >> original_program_clock_reference_base;
		AF_bit_stream >> original_program_clock_reference_reserved;
		AF_bit_stream >> original_program_clock_reference_extension;

		stuffing_bytes_length -= 6;
	}
	if (splicing_point_flag == 1)
	{
		AF_bit_stream >> splice_countdown;

		stuffing_bytes_length -= 1;
	}

	if (transport_private_data_flag == 1)
	{
		AF_bit_stream >> transport_private_data_length;
		transport_private_data = new uint8_t[transport_private_data_length.to_ulong()];
		AF_bit_stream >> transport_private_data;

		stuffing_bytes_length -= transport_private_data_length.to_ulong();
	}
	if (adaptation_field_extension_flag == 1)
	{
		AF_bit_stream >> adaptation_field_extension_length;
		AF_bit_stream >> ltw_flag;
		AF_bit_stream >> piecewise_rate_flag;
		AF_bit_stream >> seamless_splice_flag;
		AF_bit_stream >> adaptation_field_extension_reserved;

		stuffing_bytes_length -= adaptation_field_extension_length.to_ulong() - 1;

		if (ltw_flag == 1)
		{
			AF_bit_stream >> ltw_valid_flag;
			AF_bit_stream >> ltw_offset;

			stuffing_bytes_length -= 2;
		}
		if (piecewise_rate_flag == 1)
		{
			AF_bit_stream >> piecewise_rate_reserved;
			AF_bit_stream >> piecewise_rate;

			stuffing_bytes_length -= 3;
		}
		if (seamless_splice_flag == 1)
		{
			AF_bit_stream >> splice_type;
			AF_bit_stream >> DTS_next_access_unit;

			stuffing_bytes_length -= 5;
		}

	}
	return int32_t();
}

void xTS_AdaptationField::Print() const
{
	std::stringstream ss;

	ss <<
		"AF:" <<
		" L=" << std::setw(3) << adaptation_field_length.to_ulong() <<
		" DC=" << discontinuity_indicator <<
		" RA=" << random_access_indicator <<
		" SPI=" << elementary_stream_priority_indicator <<
		" PR=" << PCR_flag <<
		" OR=" << OPCR_flag <<
		" SPF=" << splicing_point_flag <<
		" TP=" << transport_private_data_flag <<
		" EX=" << adaptation_field_extension_flag;
	
	if (PCR_flag == 1)
	{
		uint32_t PCR = program_clock_reference_base.to_ulong() * xTS::BaseToExtendedClockMultiplier + program_clock_reference_extension.to_ulong();
		double time = (double)PCR / (double)xTS::ExtendedClockFrequency_Hz; //wtf it works 
		ss << " PCR=" << PCR << " (Time=" << std::setprecision(7) << time << "s)";
	}

	if (OPCR_flag == 1)
	{
		uint32_t OPCR = original_program_clock_reference_base.to_ulong() * xTS::BaseToExtendedClockMultiplier + original_program_clock_reference_extension.to_ulong();
		double time = (double)OPCR / (double)xTS::ExtendedClockFrequency_Hz; 
		ss << " OPCR=" << OPCR << " (Time=" << std::setprecision(7) << time << "s)";
	}
	ss <<" Stuffing=" << stuffing_bytes_length << " ";

	std::cout << ss.str();

}

uint32_t xTS_AdaptationField::getNumBytes() const
{
	return uint32_t();
}

uint8_t xTS_AdaptationField::getAdaptationFieldLength() const
{
	return uint8_t(adaptation_field_length.to_ulong());
}
