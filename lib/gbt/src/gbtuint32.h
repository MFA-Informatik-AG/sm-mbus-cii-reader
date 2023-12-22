/**
 * @file gbtuint32.h
 * @brief Represents a 32-bit unsigned integer value in the GBT protocol.
 * 
 * This class provides functionality to parse, clone, and convert the value to a string.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gbtvaluebase.h"

class GbtUint32 : public GbtValueBase
{
    public:
        bool parse(const uint8_t* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent) final;          
        void clone(GbtUint32 const& source);                                                                                    
        void asString(char* buffer, size_t const bufferSize) const final;                                                       
        uint32_t getValue() const;                                              
        GbtValueBase::GbtValueType getValueType() const final;                                                                          

    private:
        uint32_t _uint32Value;
};