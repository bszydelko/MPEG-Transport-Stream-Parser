#pragma once
#include "tsCommon.h"
#include "pesPacketHeader.h"
#include "tsAdaptationField.h"
#include "tsPacketHeader.h"
#include "tsTransportStream.h"

class xTS;
class xPES_PacketHeader;
class xTS_AdaptationField;
class xTS_PacketHeader;

class xPES_Assembler
{
public:
    enum class eResult : int32_t
    {
        UnexpectedPID = 1,
        StreamPackedLost,
        AssemblingStarted,
        AssemblingContinue,
        AssemblingFinished,
    };

protected:
    //setup
    int32_t  m_PID;
    //buffer
    uint8_t* m_Buffer;
    uint32_t m_BufferSize;
    uint32_t m_DataOffset;
    //operation
    int8_t            m_LastContinuityCounter;
    bool              m_Started;
    xPES_PacketHeader m_PESH;

    //file
    const char* m_Filename;
    FILE* m_File;

public:
    xPES_Assembler();
    ~xPES_Assembler();

    void    Init(int32_t PID);
    eResult AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField);

    void     PrintPESH()        const { m_PESH.Print(); }
    uint8_t* getPacket()        { return m_Buffer; }
    int32_t  getPacketLength()  const { return m_DataOffset; }
    int32_t  getHeaderLength()  const { return m_PESH.getHeaderDataLength() + xTS::PES_HeaderLength; }
    int32_t  getDataLength()    const { return m_DataOffset - this->getHeaderLength(); }

protected:
    void xBufferReset();
    void xBufferAppend(const uint8_t* Data, int32_t Size);
};
