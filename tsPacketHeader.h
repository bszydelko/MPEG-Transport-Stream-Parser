#pragma once
#include "tsCommon.h"
#include <bitset>

class xTS_PacketHeader
{
public:
    enum class ePID : uint16_t
    {
        PAT = 0x0000,
        CAT = 0x0001,
        TSDT = 0x0002,
        IPMT = 0x0003,
        NIT = 0x0010, //DVB specific PID
        SDT = 0x0011, //DVB specific PID
        NuLL = 0x1FFF,
    };

protected:
    std::bitset<8>  m_Sync_byte;
    std::bitset<1>  m_Transport_error_indicator;
    std::bitset<1>  m_Payload_unit_start_indicator;
    std::bitset<1>  m_Transport_priority;
    std::bitset<13> m_PID;
    std::bitset<2>  m_Transport_scrambling_control;
    std::bitset<2>  m_Adaptation_field_control;
    std::bitset<4>  m_Continuity_counter;

public:
    void     Reset();
    int32_t  Parse(const uint8_t* Input);
    void     Print() const;

public:
    //direct acces to header values
    uint8_t  getSyncByte()                   const;
    uint8_t  getTransportErrorIndicator()    const;
    uint8_t  getPayloadUnitStartIndicator()  const;
    uint8_t  getTransportPriority()          const;
    uint16_t getPID()                        const;
    uint8_t  getTransportScramblingControl() const;
    uint8_t  getAdaptationFieldControl()     const;
    uint8_t  getContinuityCounter()          const;

public:
    //TODO
    bool     hasAdaptationField() const { return (m_Adaptation_field_control == 0b10 || m_Adaptation_field_control == 0b11) ? true : false; }
    bool     hasPayload()         const { return (m_Adaptation_field_control == 0b01 || m_Adaptation_field_control == 0b11) ? true : false; }
};