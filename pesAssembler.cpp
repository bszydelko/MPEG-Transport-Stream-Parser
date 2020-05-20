#include "pesAssembler.h"
#include "tsPacketHeader.h"
#include "tsTransportStream.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

class xPES_Assembler;
class xTS;
enum eResult;

xPES_Assembler::xPES_Assembler()
{
	m_BufferSize = 0;
	m_Started = false;
	m_PID = -1;
	m_LastContinuityCounter = -1;
	m_DataOffset = xTS::PES_HeaderLength;

	m_Buffer = nullptr;
}

xPES_Assembler::~xPES_Assembler()
{
	delete [] m_Buffer;
}

void xPES_Assembler::Init(int32_t PID)
{
	m_PID = PID;
}

xPES_Assembler::eResult xPES_Assembler::AbsorbPacket
(
	const uint8_t* TransportStreamPacket, 
	const xTS_PacketHeader* PacketHeader, 
	const xTS_AdaptationField* AdaptationField )
{
	int32_t start_byte = 0;
	int32_t payload_size = 0;

	//do poprawy packet lost
	if (m_LastContinuityCounter != -1 
		&& (PacketHeader->getContinuityCounter() - m_LastContinuityCounter != 1 
			&& (int32_t)PacketHeader->getContinuityCounter() - m_LastContinuityCounter != -15))
	{
		xBufferReset();
		return eResult::StreamPackedLost;
	}

	if (PacketHeader->getPayloadUnitStartIndicator() == 1) //start of PES packet, parse PES header
	{
		if (m_Buffer != nullptr) xBufferReset();

		m_Started = true;
		m_LastContinuityCounter = PacketHeader->getContinuityCounter();

		start_byte = xTS::TS_HeaderLength + AdaptationField->getAdaptationFieldLength() + 1;
		payload_size =
			xTS::TS_PacketLength
			- xTS::TS_HeaderLength
			- (AdaptationField->getAdaptationFieldLength() + 1)
			- xTS::PES_HeaderLength;

		m_PESH.Parse(TransportStreamPacket, start_byte);

		xBufferAppend(&TransportStreamPacket[start_byte], payload_size);
		return eResult::AssemblingStarted;
	}

	if (m_Started) //still assembling 
	{
		m_LastContinuityCounter = PacketHeader->getContinuityCounter();

		if (PacketHeader->hasAdaptationField()) //maybe last PES packet part
		{
			start_byte = xTS::TS_HeaderLength + AdaptationField->getAdaptationFieldLength() + 1;
			payload_size =
				xTS::TS_PacketLength
				- xTS::TS_HeaderLength
				- (AdaptationField->getAdaptationFieldLength() + 1);
		}
		else 
		{
			start_byte = xTS::TS_HeaderLength;
			payload_size = xTS::TS_PacketLength - xTS::TS_HeaderLength - (AdaptationField->getAdaptationFieldLength());
		}
		
		xBufferAppend(&TransportStreamPacket[start_byte], payload_size);

		if (m_PESH.getPacketLength() == m_BufferSize) //last PES packet part
		{
			m_Started = false;
			return eResult::AssemblingFinished;
		}

		return eResult::AssemblingContinue;
	}
}

void xPES_Assembler::xBufferReset()
{
	m_BufferSize = 0;
	m_DataOffset = xTS::PES_HeaderLength;

	delete [] m_Buffer;
	m_Buffer = nullptr;
}

void xPES_Assembler::xBufferAppend(const uint8_t* Data, int32_t Size)
{
	m_BufferSize += Size;
	m_DataOffset += Size;

	if (m_Buffer == nullptr) //after reset
	{
		m_Buffer = new uint8_t[m_BufferSize];

		for (int32_t byte = 0; byte < m_BufferSize; byte++)
		{
			m_Buffer[byte] = Data[byte];
		}
		return;
	}
	
	uint8_t* temp_buffer = new uint8_t[m_BufferSize];

	int32_t byte = 0;
	for (byte = 0; byte < m_BufferSize - Size; byte++)
	{
		temp_buffer[byte] = m_Buffer[byte];
	}

	for (int32_t new_byte = 0; new_byte < Size; byte++, new_byte++)
	{
		temp_buffer[byte] = Data[new_byte];
	}

	delete [] m_Buffer;
	m_Buffer = new uint8_t[m_BufferSize];

	for (size_t byte = 0; byte < m_BufferSize; byte++)
	{
		m_Buffer[byte] = temp_buffer[byte];
	}

	delete [] temp_buffer;
	//std::istringstream PES_buffer_bit_stream(xTS::getBitStream(Data, 0, Size));
}