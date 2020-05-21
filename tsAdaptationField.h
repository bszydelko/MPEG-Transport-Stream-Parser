#pragma once
#include "tsCommon.h"
#include <bitset>

class xTS_AdaptationField
{
public:
    xTS_AdaptationField();
protected:
    //AF lenght
    std::bitset<8>  adaptation_field_length;
    //mandatory field
    std::bitset<1>  discontinuity_indicator;
    std::bitset<1>  random_access_indicator;
    std::bitset<1>  elementary_stream_priority_indicator;
    std::bitset<1>  PCR_flag;
    std::bitset<1>  OPCR_flag;
    std::bitset<1>  splicing_point_flag;
    std::bitset<1>  transport_private_data_flag;
    std::bitset<1>  adaptation_field_extension_flag;

    //zad dod 1 - optional fields
    //PCR
    std::bitset<33> program_clock_reference_base;
    std::bitset<6>  program_clock_reference_reserved;
    std::bitset<9>  program_clock_reference_extension;
    //OPCR
    std::bitset<33> original_program_clock_reference_base;
    std::bitset<6>  original_program_clock_reference_reserved;
    std::bitset<9>  original_program_clock_reference_extension;

    std::bitset<8>  splice_countdown;

    std::bitset<8>  transport_private_data_length;
    uint8_t*        transport_private_data;
    //AdaptationFieldExtension
    std::bitset<8>  adaptation_field_extension_length;
    std::bitset<1>  ltw_flag;
    std::bitset<1>  piecewise_rate_flag;
    std::bitset<1>  seamless_splice_flag;
    std::bitset<5>  adaptation_field_extension_reserved;
    //LTW
    std::bitset<1>  ltw_valid_flag;
    std::bitset<15> ltw_offset;
    //PiecewiseRate
    std::bitset<2>  piecewise_rate_reserved;
    std::bitset<22> piecewise_rate;
    //SeamlessSplice
    std::bitset<4>  splice_type;
    std::bitset<36> DTS_next_access_unit; //do it later

    //TODO - stuffing bytes
    size_t          stuffing_bytes_length;
    uint8_t*        stuffing_bytes;

public:
    void    Reset();
    int32_t Parse(const uint8_t* Input, uint8_t AdapdationFieldControl);
    void    Print() const;

public:
    //derrived values
    uint32_t getNumBytes() const;
    uint8_t  getAdaptationFieldLength() const;
};