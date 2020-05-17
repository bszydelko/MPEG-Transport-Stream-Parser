#include "tsAdaptationField.h"
#include "tsTransportStream.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

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

	//potrzeba sprawdzenia na "bogatszym" w pola pakiet
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
		" AF:" <<
		" L=" << std::setw(3) << adaptation_field_length.to_ulong() <<
		" DC=" << discontinuity_indicator <<
		" RA=" << random_access_indicator <<
		" SPI=" << elementary_stream_priority_indicator <<
		" PR=" << PCR_flag <<
		" OR=" << OPCR_flag <<
		" SPF=" << splicing_point_flag <<
		" TP=" << transport_private_data_flag <<
		" EX=" << adaptation_field_extension_flag;

	std::cout << ss.str();

}
