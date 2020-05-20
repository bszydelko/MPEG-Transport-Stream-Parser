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
    std::bitset<24> m_Packet_start_code_prefix;
    std::bitset<8>  m_Stream_id;
    std::bitset<16> m_PES_packet_length;
    //Optional PES header - zad dod 2
    std::bitset<2> m_Marker_bits;
    std::bitset<2> m_PES_scrambling_control;
    std::bitset<1> m_PES_priority;
    std::bitset<1> m_Data_alignment_indicator;
    std::bitset<1> m_Copyright;
    std::bitset<1> m_Original_or_copy;
    std::bitset<2> m_PTS_DTS_flags;
    std::bitset<1> m_ESCR_flag;
    std::bitset<1> m_ES_rate_flag;
    std::bitset<1> m_DSM_trick_mode_flag;
    std::bitset<1> m_Additional_copy_info_flag;
    std::bitset<1> m_PES_CRC_flag;
    std::bitset<1> m_PES_extension_flag;
    std::bitset<8> m_PES_header_data_length;

    std::bitset<40> m_PTS_data;
    std::bitset<40> m_DTS_data;

    std::bitset<48> m_ESCR_data;

    std::bitset<24> m_ES_rate_data;

public:
    void     Reset();
    int32_t  Parse(const uint8_t* Input, size_t start_byte);
    void     Print() const;

public:
    //PES packet header
    uint32_t getPacketStartCodePrefix() const { return m_Packet_start_code_prefix.to_ulong(); }
    uint8_t  getStreamId()              const { return m_Stream_id.to_ulong(); }
    uint16_t getPacketLength()          const { return m_PES_packet_length.to_ulong(); }
};