/**
 * @file gbtuint16.h
 * @brief Represents a 16-bit unsigned integer value in the GBT protocol.
 * 
 * This class provides functionality to parse, convert to string, and retrieve the value of a 16-bit unsigned integer
 * in the GBT protocol. It inherits from the GbtValueBase class.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gbtvaluebase.h"

class GbtUint16 : public GbtValueBase
{
    public:
        bool parse(const uint8_t* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent) final;              
        void asString(char* buffer, size_t const bufferSize) const final;                                                           
        uint16_t getValue() const;                                                                                                  
        GbtValueBase::GbtValueType getValueType() const final; 

    private:
        uint16_t _uint16Value;
};