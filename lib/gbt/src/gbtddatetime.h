/**
 * @file gbtdatetime.h
 * @brief Represents a date and time in the GBT protocol.
 * 
 * This class provides methods to parse GBT date and time data, clone the object, 
 * convert it to a string representation, and obtain the Unix timestamp.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "gbtvaluebase.h"

class GbtDateTime : public GbtValueBase
{
    public:
        bool parse(uint8_t const* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent) final;         
        void asString(char* buffer, size_t const bufferSize) const final;                                                      
        GbtValueBase::GbtValueType getValueType() const final; 
        void clone(GbtDateTime const& source);                                  

        time_t asUnixTimeStamp() const;                                         // converts the GbtDateTime object to a Unix timestamp
        uint16_t getYear() const;                                               // returns the year
        uint8_t getMonth() const;                                               // returns the month
        uint8_t getDay() const;                                                 // returns the day
        uint8_t getHour() const;                                                // returns the hour
        uint8_t getMinute() const;                                              // returns the minute
        uint8_t getSecond() const;                                              // returns the second

    private:
        uint16_t m_year;                                                          
        uint8_t m_month;                  
        uint8_t m_day;
        uint8_t m_hour;
        uint8_t m_minute;
        uint8_t m_second;
};