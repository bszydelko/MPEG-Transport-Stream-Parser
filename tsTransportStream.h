#pragma once
#include "tsCommon.h"
#include <string>
#include <bitset>
#include <sstream>

/*
MPEG-TS packet:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |                             Header                            | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   4 |                  Adaptation field + Payload                   | `
`     |                                                               | `
` 184 |                                                               | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `


MPEG-TS packet header:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |       SB      |E|S|T|           PID           |TSC|AFC|   CC  | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `

Sync byte                    (SB ) :  8 bits
Transport error indicator    (E  ) :  1 bit
Payload unit start indicator (S  ) :  1 bit
Transport priority           (T  ) :  1 bit
Packet Identifier            (PID) : 13 bits
Transport scrambling control (TSC) :  2 bits
Adaptation field control     (AFC) :  2 bits
Continuity counter           (CC ) :  4 bits
*/


//=============================================================================================================================================================================

class xTS
{
public:
  static constexpr uint32_t TS_PacketLength = 188;
  static constexpr uint32_t TS_HeaderLength = 4;

  static constexpr uint32_t PES_HeaderLength = 6;

  static constexpr uint32_t BaseClockFrequency_Hz         =    90000; //Hz
  static constexpr uint32_t ExtendedClockFrequency_Hz     = 27000000; //Hz
  static constexpr uint32_t BaseClockFrequency_kHz        =       90; //kHz
  static constexpr uint32_t ExtendedClockFrequency_kHz    =    27000; //kHz
  static constexpr uint32_t BaseToExtendedClockMultiplier =      300;


  //my stuff
  static std::string getBitStream(const uint8_t* _Input, size_t _startByte, size_t _count);
  static constexpr uint32_t TS_AFLengthByte = 4; //of course it's 5
  
};

//=============================================================================================================================================================================

class xTS_PacketHeader
{
public:
  enum class ePID : uint16_t
  {
    PAT  = 0x0000,
    CAT  = 0x0001,
    TSDT = 0x0002,
    IPMT = 0x0003,
    NIT  = 0x0010, //DVB specific PID
    SDT  = 0x0011, //DVB specific PID
    NuLL = 0x1FFF,
  };

protected:
    std::bitset<8>  sync_byte;
    std::bitset<1>  transport_error_indicator;
    std::bitset<1>  payload_unit_start_indicator;
    std::bitset<1>  transport_priority;
    std::bitset<13> PID;
    std::bitset<2>  transport_scrambling_control;
    std::bitset<2>  adaptation_field_control;
    std::bitset<4>  continuity_counter;

public:
  void     Reset();
  int32_t  Parse(const uint8_t* Input);
  void     Print() const;

public:
  //direct acces to header values
    uint8_t getSyncByte() const;
    uint8_t getTransportErrorIndicator() const;
    uint8_t getPayloadUnitStartIndicator() const;
    uint8_t getTransportPriority() const;
    uint8_t getPID() const;
    uint8_t getTransportScramblingControl() const;
    uint8_t getAdaptationFieldControl() const;
    uint8_t getContinuityControl() const;

public:
  //TODO
    bool     hasAdaptationField() const { return (adaptation_field_control.to_ulong() == 2 || adaptation_field_control.to_ulong() == 3) ? true : false; }
    bool     hasPayload() const { adaptation_field_control.to_ulong() == 3 ? true : false; }
};

//=============================================================================================================================================================================

class xTS_AdaptationField
{
protected:
    //AF lenght
    std::bitset<8>adaptation_field_length;

    //mandatory field
    std::bitset<1>discontinuity_indicator;
    std::bitset<1>random_access_indicator;
    std::bitset<1>elementary_stream_priority_indicator;
    std::bitset<1>PCR_flag;
    std::bitset<1>OPCR_flag;
    std::bitset<1>splicing_point_flag;
    std::bitset<1>transport_private_data_flag;
    std::bitset<1>adaptation_field_extension_flag;

    //zad dod 1 - optional fields
    //PCR
    std::bitset<33> program_clock_reference_base;
    std::bitset<6> program_clock_reference_reserved;
    std::bitset<9> program_clock_reference_extension;
    //OPCR
    std::bitset<48> original_program_clock_reference_base;
    std::bitset<6> original_program_clock_reference_reserved;
    std::bitset<9> original_program_clock_reference_extension;
    
    std::bitset<8> splice_countdown;

    std::bitset<8> transport_private_data_length;
    uint8_t* transport_private_data;
    //AdaptationFieldExtension
    std::bitset<8> adaptation_field_extension_length;
    std::bitset<1> ltw_flag;
    std::bitset<1> piecewise_rate_flag;
    std::bitset<1> seamless_splice_flag;
    std::bitset<5> adaptation_field_extension_reserved;
    //LTW
    std::bitset<1> ltw_valid_flag;
    std::bitset<15> ltw_offset;
    //PiecewiseRate
    std::bitset<2> piecewise_rate_reserved;
    std::bitset<22> piecewise_rate;
    //SeamlessSplice
    std::bitset<4> splice_type;
    std::bitset<36> DTS_next_access_unit; //do it later

    int N; //nieprzerwana liczba pakietów z tym samym PID

    //TODO - stuffing bytes

    uint8_t* StuffingBytes;



public:
    void Reset();
    int32_t Parse(const uint8_t* Input, uint8_t AdapdationFieldControl);
    void Print() const;


public:
    //derrived values
    uint32_t getNumBytes() const {}
};

//=============================================================================================================================================================================


class xPES_PacketHeader
{
public:
    enum eStreamId : uint8_t
    {
        eStreamId_program_stream_map = 0xBC,
        eStreamId_padding_stream = 0xBE,
        eStreamId_private_stream_2 = 0xBF,
        eStreamId_ECM = 0xF0,
        eStreamId_EMM = 0xF1,
        eStreamId_program_stream_directory = 0xFF,
        eStreamId_DSMCC_stream = 0xF2,
        eStreamId_ITUT_H222_1_type_E = 0xF8,
    };

protected:
    //PES packet header
    std::bitset<24> m_PacketStartCodePrefix;
    std::bitset<8>  m_StreamId;
    std::bitset<16> m_PacketLength;

public:
    void     Reset();
    int32_t  Parse(const uint8_t* Input);
    void     Print() const;

public:
    //PES packet header
    uint32_t getPacketStartCodePrefix() const { return m_PacketStartCodePrefix.to_ulong(); }
    uint8_t  getStreamId() const { return m_StreamId.to_ulong(); }
    uint16_t getPacketLength() const { return m_PacketLength.to_ulong(); }
};

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

public:
    xPES_Assembler();
    ~xPES_Assembler();

    void    Init(int32_t PID);
    eResult AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField);

    void     PrintPESH() const { m_PESH.Print(); }
    uint8_t* getPacket() { return m_Buffer; }
    int32_t  getNumPacketBytes() const { return m_DataOffset; }

protected:
    void xBufferReset();
    void xBufferAppend(const uint8_t* Data, int32_t Size);
};