#include "pesAssembler.h"
#include "tsPacketHeader.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

class xPES_Assembler;


void xPES_Assembler::Init(int32_t PID)
{
	m_LastContinuityCounter = -1;
	m_PID = PID;
}

xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField)
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