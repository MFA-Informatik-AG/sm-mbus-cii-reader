/**
 * @file gbtdata.h
 * @brief This file contains the declaration of the GbtData class.
 * 
 * The GbtData class represents a data structure that holds GBT (Generic Binary Telemetry) values.
 * It provides methods for parsing data, accessing values, and retrieving metadata such as date and time.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gbtvaluebase.h"
#include "gbtddatetime.h"
#include "gbtuint32.h"

class GbtData
{
    private:
        static const uint8_t MAX_GBTVALUES = 100;                   // maximum number of single GBT values (e.g. int, float, string, etc.)
        static const uint8_t MAX_STRUCTURE_NESTED = 20;             // maximum number of nested structures (GBT protocol)
        static const uint8_t MAX_ARRAY_NESTED = 20;                 // maximum number of nested arrays (GBT protocol)
        
        GbtValueBase* m_gbtValues[MAX_GBTVALUES];                   // array of pointers to GBT values (m_gbtValueCount is the number of valid pointers in the array)
        uint8_t m_gbtValueCount = 0;                                // number of valid pointers in the array
        GbtDateTime m_dateAndTime;                                  // date and time of the GBT data
        GbtUint32 m_longInvokedPriorityId;                          // invoked priority ID of the GBT data

    public:
        GbtData();                                                  
        ~GbtData();
        int parse(uint8_t const* data, size_t const size);          // parses the received GBT data into single values
        uint8_t getValueCount() const;                              // returns the number of single GBT values
        GbtDateTime const& getDateTime() const;                     // returns the date and time of the GBT data
        GbtUint32 const& getLongInvokedPriorityId() const;          // returns the invoked priority ID of the GBT data
        GbtValueBase const* getValue(uint8_t index) const;          // returns a pointer to the single GBT value at the given index
};