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
	std::istringstream H_bit_stream(xTS::getBitStream(Input, 0, xTS::TS_HeaderLength));

	H_bit_stream >> SB >> E >> S >> P >> PID >> TSC >> AFC >> CC;

	return int32_t();
}

void xTS_PacketHeader::Print() const
{
	std::stringstream oss;

	oss <<
		"TS:"   <<
		" SB="					  << SB  .to_ulong() <<
		" E="					  << E   .to_ulong() <<
		" S="					  << S   .to_ulong() <<
		" P="					  << P   .to_ulong() <<
		" PID=" << std::setw(4)   << PID .to_ulong() <<
		" TSC="					  << TSC .to_ulong() <<
		" AFC="					  << AFC .to_ulong() <<
		" CC="  << std::setw(2)   << CC  .to_ulong();

	std::cout << oss.str();
}
uint16_t xTS_PacketHeader::getAFC() const
{
	return uint16_t(AFC.to_ulong());
}
//=============================================================================================================================================================================




//=============================================================================================================================================================================
// xTS_AdaptationField
//=============================================================================================================================================================================
void xTS_AdaptationField::Reset()
{
	AFL.reset();
	DC.reset();
	RA.reset();
	SPI.reset();
	PR.reset();
	OR.reset();
	SPF.reset();
	TP.reset();
	EX.reset();

	junk.reset();
}

int32_t xTS_AdaptationField::Parse(const uint8_t* Input, uint8_t AdapdationFieldControl)
{
	//Improve it
	int lengthAF = Input[xTS::TS_HeaderLength];
	AFL = lengthAF;

	lengthAF++;

	std::istringstream AF_bit_stream(xTS::getBitStream(Input, xTS::TS_HeaderLength + 1, lengthAF));

	AF_bit_stream >> DC >> RA >> SPI >> PR >> OR >> SPF >> TP >> EX >> junk;

	return int32_t();
}

void xTS_AdaptationField::Print() const
{
	std::stringstream oss;

	oss <<
		" AF:"  <<
		" L="   << std::setw(3) << AFL.to_ulong() <<
		" DC="  << DC  <<
		" RA="  << RA  <<
		" SPI=" << SPI <<
		" PR="  << PR  <<
		" OR="  << OR  <<
		" SPF=" << SPF <<
		" TP="  << TP  <<
		" EX="  << EX;

	std::cout << oss.str();

}






//=============================================================================================================================================================================

