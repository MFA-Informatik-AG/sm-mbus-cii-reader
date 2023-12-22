/**
 * @file gbtvaluebase.h
 * @brief Defines the GbtValueBase class, which is a base class for GBT values.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

class GbtValueBase
{
    public:
        enum class GbtValueType
        {
            GBTVALUETYPE_NONE = 0,
            GBTVALUETYPE_UINT8 = 1,
            GBTVALUETYPE_UINT16 = 2,
            GBTVALUETYPE_UINT32 = 3,
            GBTVALUETYPE_OCTETESTRING =4
        };

        GbtValueBase() : m_structureIdent(0), m_arrayIdent(0) {};
        virtual ~GbtValueBase() = 0;

        virtual bool parse(const uint8_t* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent) = 0;        // parse the data and return the number of bytes parsed
        virtual void asString(char* buffer, size_t const bufferSize) const = 0;                                                     // return the value as a string (debugging purposes)
        virtual GbtValueBase::GbtValueType getValueType() const = 0;                                                                // return the value type

        uint8_t getStructureIdent() const;                                                                                          // return the structure identifier            
        uint8_t getArrayIdent() const;                                                                                              // return the array identifier

    protected:

        uint8_t m_structureIdent;
        uint8_t m_arrayIdent;
};