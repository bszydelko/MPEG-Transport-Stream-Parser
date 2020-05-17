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
