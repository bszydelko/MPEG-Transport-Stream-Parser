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

	m_File = nullptr;
	m_Filename = "";

	m_Buffer = NULL;
}

xPES_Assembler::~xPES_Assembler()
{
	fclose(m_File);
	delete [] m_Buffer;
}

void xPES_Assembler::Init(int32_t PID, std::string file_extension)
{
	m_PID = PID;
	std::string name_builder = "../PID" + std::to_string(PID) + file_extension;
	m_Filename = name_builder.c_str();
	m_File = fopen(m_Filename, "wb");
}

xPES_Assembler::eResult xPES_Assembler::AbsorbPacket
(
	const uint8_t* TransportStreamPacket, 
	const xTS_PacketHeader* PacketHeader, 
	const xTS_AdaptationField* AdaptationField )
{
	int32_t start_byte = 0;
	int32_t payload_size = 0;

		if (m_LastContinuityCounter != -1
			&& (PacketHeader->getContinuityCounter() - m_LastContinuityCounter != 1
				&& (int32_t)PacketHeader->getContinuityCounter() - m_LastContinuityCounter != -15))
		{
			xBufferReset();
			return eResult::StreamPackedLost;
		}

		//start of PES packet, parse PES header
		if (PacketHeader->getPayloadUnitStartIndicator() == 1)
		{
			if (m_Started && m_PESH.getPacketLength() == 0)
			{
				m_Started = false;
				size_t writed = fwrite(&m_Buffer[this->getHeaderLength()], sizeof(char), this->getDataLength(), m_File);
				xBufferReset();
			}

			if (m_Buffer != nullptr) xBufferReset();

			m_Started = true;
			m_LastContinuityCounter = PacketHeader->getContinuityCounter();

			start_byte = xTS::TS_HeaderLength + AdaptationField->getAdaptationFieldLength() + 1;
			payload_size =
				xTS::TS_PacketLength
				- xTS::TS_HeaderLength
				- (AdaptationField->getAdaptationFieldLength() + 1);
			if (AdaptationField->getAdaptationFieldLength() == 0)
			{
				start_byte--;
				payload_size++;
			}

			m_PESH.Parse(&TransportStreamPacket[start_byte]);

			xBufferAppend(&TransportStreamPacket[start_byte], payload_size);
			return eResult::AssemblingStarted;
		}

		//still assembling 
		if (m_Started)
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
				payload_size = xTS::TS_PacketLength - xTS::TS_HeaderLength;
			}

			xBufferAppend(&TransportStreamPacket[start_byte], payload_size);

			//last PES packet part, length without header
			if (m_PESH.getPacketLength() == m_BufferSize - xTS::PES_HeaderLength)
			{
				m_Started = false;
				size_t writed = fwrite(&m_Buffer[this->getHeaderLength()], sizeof(char), this->getDataLength(), m_File);
				return eResult::AssemblingFinished;
			}

			return eResult::AssemblingContinue;
		}
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

	if (m_Buffer == nullptr) //after reset
	{
		m_Buffer = new uint8_t[m_BufferSize];
		std::memcpy(m_Buffer, Data, m_BufferSize);
		return;
	}
	
	uint8_t* temp_buffer = new uint8_t[m_BufferSize];

	int32_t new_data_start_byte = m_BufferSize - Size;

	std::memcpy(temp_buffer, m_Buffer, new_data_start_byte);
	std::memcpy(&temp_buffer[new_data_start_byte], Data, Size); //append new data

	delete [] m_Buffer;
	m_Buffer = new uint8_t[m_BufferSize];

	std::memcpy(m_Buffer, temp_buffer, m_BufferSize);//fill buffer

	delete [] temp_buffer;
}