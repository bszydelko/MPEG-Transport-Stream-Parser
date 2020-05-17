#pragma once
#include <bitset>


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
    int32_t  Parse(const uint8_t* Input, size_t start_byte);
    void     Print() const;

public:
    //PES packet header
    uint32_t getPacketStartCodePrefix() const { return m_PacketStartCodePrefix.to_ulong(); }
    uint8_t  getStreamId() const { return m_StreamId.to_ulong(); }
    uint16_t getPacketLength() const { return m_PacketLength.to_ulong(); }
};