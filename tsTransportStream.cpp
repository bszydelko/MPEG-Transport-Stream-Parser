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

	for (size_t byte = _startByte; byte < _count; byte++)
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
	SB.reset();
	E.reset();
	S.reset();
	P.reset();
	PID.reset();
	TSC.reset();
	AFC.reset();
	CC.reset();
}

int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
	std::istringstream bit_stream(xTS::getBitStream(Input, 0, xTS::TS_HeaderLength));

	bit_stream >> SB >> E >> S >> P >> PID >> TSC >> AFC >> CC;

	return int32_t();
}

void xTS_PacketHeader::Print() const
{
	std::stringstream oss;

	oss <<
		"TS:" <<
		" SB=" << std::to_string(SB.to_ulong()) <<
		" E=" << std::to_string(E.to_ulong()) <<
		" S=" << std::to_string(S.to_ulong()) <<
		" P=" << std::to_string(P.to_ulong()) <<
		" PID=" << std::setw(4) << std::to_string(PID.to_ulong()) <<
		" TSC=" << std::to_string(TSC.to_ulong()) <<
		" AFC=" << std::to_string(AFC.to_ulong()) <<
		" CC=" << std::setw(2) << std::to_string(CC.to_ulong());

	std::cout << oss.str();
}
//=============================================================================================================================================================================




//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================



//=============================================================================================================================================================================