/**
 * @file smcayenne.h
 * @brief This file contains the declaration of the SmCayenne class, which is a subclass of CayenneLPP.
 *        It provides additional functionality for encoding and decoding data in the Cayenne Low Power Payload (LPP) format.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <CayenneLPP.h>

#include "gbtvaluebase.h"
#include "appconfig.h"

class SmCayenne : public CayenneLPP
{
    private:
        static uint8_t const SMDECODE_CHANNEL_MEASUREINTERVAL = 0x0a;                               // identifier for the measure interval 
        static uint8_t const SMDECODE_CHANNEL_SENDDATATYPE = 0x0b;                                  // identifier for the send data type
        static uint8_t const SMDECODE_CHANNEL_DECRYPTDATA = 0x0c;                                   // identifier for the decrypt data
        static uint8_t const SMDECODE_CHANNEL_AUTHENTICATIONKEY = 0x0d;                             // identifier for the authentication key
        static uint8_t const SMDECODE_CHANNEL_AESKEY = 0x0e;                                        // identifier for the AES key
        static uint8_t const SMDECODE_CHANNEL_AESIV = 0x0f;                                         // identifier for the AES IV 
        static uint8_t const SMDECODE_CHANNEL_CYCLETIMEOUT = 0x10;                                  // identifier for the cycle timeout

        static uint8_t const SMDECODE_UINT8 = 1;                                                    // identifier for the uint8_t data type when receiving application settings
        static uint8_t const SMDECODE_UINT32 = 4;                                                   // identifier for the uint32_t data type when receiving application settings
        static uint8_t const SMDECODE_BOOL = 6;                                                     // identifier for the bool data type when receiving application settings
        static uint8_t const SMDECODE_BYTEARRAY = 7;                                                // identifier for the byte array data type when receiving application settings

        static uint8_t const SMLPP_BATTERYVOLTAGE = 200;                                            // identifier for the battery voltage
        static uint8_t const SMLPP_READLOOPS = 201;                                                 // identifier for the read loops
        static uint8_t const SMLPP_SENDFAILURES = 202;                                              // identifier for the send failures

        static uint8_t const SMLPP_UNKNOWN_VALUETYPE = 0;                                           // identifier for the LPP unknown data type
        static uint8_t const SMLPP_UINT8_VALUETYPE = 1;                                             // identifier for the LPP uint8_t data type
        static uint8_t const SMLPP_UINT16_VALUETYPE = 2;                                            // identifier for the LPP uint16_t data type
        static uint8_t const SMLPP_UINT32_VALUETYPE = 3;                                            // identifier for the LPP uint32_t data type
        static uint8_t const SMLPP_OCTETESTRING_VALUETYPE = 4;                                      // identifier for the LPP octet string data type

        static uint8_t const SMLPP_UINT8_SIZE = 1;                                                  // size of the LPP uint8_t data type in bytes
        static uint8_t const SMLPP_UINT16_SIZE = 2;                                                 // size of the LPP uint16_t data type in bytes
        static uint8_t const SMLPP_UINT32_SIZE = 4;                                                 // size of the LPP uint32_t data type in bytes
        static uint8_t const SMLPP_HEADER_SIZE = 3;                                                 // size of the LPP header in bytes (channel, type, size)

        uint8_t m_lppIndex;                                                                         // index of the next LPP value to be added
        uint8_t getLppSize(GbtValueBase const* gbtValue) const;                                     // get the size of the LPP value
        uint8_t getLppValueType(GbtValueBase const* gbtValue) const;                                // get the type of the LPP value
        bool appendLppValue(GbtValueBase const* gbtValue);                                          // append the LPP value to the buffer
        uint32_t getUint32FromByteArray(uint8_t const* data, size_t& offset);                       // get a uint32_t from the byte array, offset is incremented
        uint8_t getUint8FromByteArray(uint8_t const* data, size_t& offset);                         // get a uint8_t from the byte array, offset is incremented
        bool getBoolFromByteArray(uint8_t const* data, size_t& offset);                             // get a bool from the byte array, offset is incremented

    public:
	    SmCayenne(uint8_t size) : CayenneLPP(size) {}                                               // constructor exeted by the base class
        void reset();                                                                               // reset the LPP buffer
        uint8_t addSmData(uint8_t channel, GbtValueBase const* gbtValue);                           // add a GbtValueBase to the LPP buffer
        uint8_t addBatteryVoltage(uint8_t channel, uint16_t value);                                 // add the battery voltage to the LPP buffer
        uint8_t addSendFailures(uint8_t channel, uint16_t value);                                   // add the send failures to the LPP buffer
        uint8_t addSendReadLoops(uint8_t channel, uint32_t value);                                  // add the read loops to the LPP buffer
        void smDecodeReceivedAppSettings(uint8_t const* data, size_t size, AppConfig& config);      // decode the received application settings
};
