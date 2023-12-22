/**
 * @file gbtuint8.h
 * @brief This file contains the declaration of the GbtUint8 class.
 * 
 * The GbtUint8 class is a derived class of GbtValueBase and represents an 8-bit unsigned integer value in the GBT protocol.
 * It provides methods for parsing, converting to string, and comparing with octet strings.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gbtvaluebase.h"

class GbtUint8 : public GbtValueBase
{
    public:
        bool parse(uint8_t const* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent) final;         
        void asString(char* buffer, size_t const bufferSize) const final;                                                      
        GbtValueBase::GbtValueType getValueType() const final; 
        uint8_t getValue() const;                                                                                              

    private:
        uint8_t _uint8Value;
};