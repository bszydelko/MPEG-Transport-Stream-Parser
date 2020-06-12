#pragma once
#include "tsCommon.h"
#include <bitset>

class xTS_AdaptationField
{
public:
    xTS_AdaptationField();
protected:
    //AF lenght
    std::bitset<8>  m_Adaptation_field_length;
    //mandatory field
    std::bitset<1>  m_Discontinuity_indicator;
    std::bitset<1>  m_Random_access_indicator;
    std::bitset<1>  m_Elementary_stream_priority_indicator;
    std::bitset<1>  m_PCR_flag;
    std::bitset<1>  m_OPCR_flag;
    std::bitset<1>  m_Splicing_point_flag;
    std::bitset<1>  m_Transport_private_data_flag;
    std::bitset<1>  m_Adaptation_field_extension_flag;

    //zad dod 1 - optional fields
    //PCR
    std::bitset<33> m_Program_clock_reference_base;
    std::bitset<6>  m_Program_clock_reference_reserved;
    std::bitset<9>  m_Program_clock_reference_extension;
    //OPCR
    std::bitset<33> m_Original_program_clock_reference_base;
    std::bitset<6>  m_Original_program_clock_reference_reserved;
    std::bitset<9>  m_Original_program_clock_reference_extension;

    std::bitset<8>  m_Splice_countdown;

    std::bitset<8>  m_Transport_private_data_length;
    uint8_t*        m_Transport_private_data;
    //AdaptationFieldExtension
    std::bitset<8>  m_Adaptation_field_extension_length;
    std::bitset<1>  m_Ltw_flag;
    std::bitset<1>  m_Piecewise_rate_flag;
    std::bitset<1>  m_Seamless_splice_flag;
    std::bitset<5>  m_Adaptation_field_extension_reserved;
    //LTW
    std::bitset<1>  m_Ltw_valid_flag;
    std::bitset<15> m_Ltw_offset;
    //PiecewiseRate
    std::bitset<2>  m_Piecewise_rate_reserved;
    std::bitset<22> m_Piecewise_rate;
    //SeamlessSplice
    std::bitset<4>  m_Splice_type;
    std::bitset<36> m_DTS_next_access_unit; //do it later

    //TODO - stuffing bytes
    size_t          m_Stuffing_bytes_length;
    uint8_t*        m_Stuffing_bytes;

public:
    void    Reset();
    int32_t Parse(const uint8_t* Input, uint8_t AdapdationFieldControl);
    void    Print() const;

public:
    //derrived values
    uint32_t getNumBytes() const;
    uint8_t  getAdaptationFieldLength() const;
};