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
	m_DataOffset = 0;

	m_Buffer = nullptr;
}

xPES_Assembler::~xPES_Assembler()
{
	delete [] m_Buffer;
}

void xPES_Assembler::Init(int32_t PID)
{
	m_LastContinuityCounter = -1;
	m_PID = PID;
}

xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField)
{

	int32_t start_byte = 0;

	if (PacketHeader->getPID() == m_PID && PacketHeader->hasPayload())
	{
		if (PacketHeader->getPayloadUnitStartIndicator() == 1)
		{
			if(m_Buffer != nullptr) xBufferReset();

			if (PacketHeader->getContinuityCounter() == 0) //parse header
			{
				m_Started = true;
				m_LastContinuityCounter = PacketHeader->getContinuityCounter();

				if (PacketHeader->hasAdaptationField()) 
				{
					start_byte = xTS::TS_HeaderLength + 1 + AdaptationField->getAdaptationFieldLength();
					m_PESH.Parse(TransportStreamPacket, start_byte);
					

				}
				else
				{
					start_byte = xTS::TS_HeaderLength;
					m_PESH.Parse(TransportStreamPacket, start_byte);
				}

				m_DataOffset += start_byte;
				xBufferAppend(&TransportStreamPacket[start_byte], xTS::TS_PacketLength - start_byte);
				return eResult::AssemblingStarted;
			}
		}
		else
		{
			start_byte = xTS::TS_HeaderLength;

			//not sure if CC max is always 15
			if (PacketHeader->getContinuityCounter() == 15)
			{
				m_Started = false;
				xBufferAppend(&TransportStreamPacket[start_byte], xTS::TS_PacketLength - start_byte);
				return eResult::AssemblingFinished;
			}

			if (PacketHeader->getContinuityCounter() - m_LastContinuityCounter == 1)
			{
				m_LastContinuityCounter = PacketHeader->getContinuityCounter();
				
				xBufferAppend(&TransportStreamPacket[start_byte], xTS::TS_PacketLength - start_byte);

				return eResult::AssemblingContinue;
			}
			else
				return eResult::StreamPackedLost;

			
		}
	}

	return eResult();
}

void xPES_Assembler::xBufferReset()
{
	m_BufferSize = 0;
	m_DataOffset = 0;

	delete [] m_Buffer;
	m_Buffer = nullptr;
}

void xPES_Assembler::xBufferAppend(const uint8_t* Data, int32_t Size)
{

	m_BufferSize += Size;
	m_DataOffset += Size;

	if (m_Buffer == nullptr)
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