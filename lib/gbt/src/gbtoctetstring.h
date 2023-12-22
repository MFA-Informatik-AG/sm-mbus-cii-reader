/**
 * @file gbtoctetstring.h
 * @brief Represents an octet string value in the GBT protocol.
 * 
 * This class inherits from GbtValueBase and provides functionality to parse, convert to string,
 * and compare octet string values.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gbtvaluebase.h"

class GbtOctetString : public GbtValueBase
{
    public:
        bool parse(uint8_t const* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent) final;          
        void asString(char* buffer, size_t const bufferSize) const final;                                                       
        GbtValueBase::GbtValueType getValueType() const final; 
        bool compareOcteteString(char const* compareWith) const;                                                                // compares the GBT value with an octet string
        char const* getStringValue() const;                                                                                     // returns the string value
        size_t getStringLength() const;                                                                                         // returns the string length 

    private:
        static const size_t MAX_GBTSTRINGSIZE = 32;

        char _stringValue[MAX_GBTSTRINGSIZE];

};