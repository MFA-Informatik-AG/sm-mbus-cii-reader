/**
 * @file appconfig.h
 * @brief Header file for the application configuration. 
 * 
 * This file contains the declaration of the application configuration.
 * 
 * @version 1.1.0
 * @author MFA Informatik AG, Andreas Schneider
 * @date 23.01.2024
 * 
 * Version History:
 * 1.1.0    23.01.2024  Added support for ESP32/WLAN EP connection, refactored measurement into apiTimer
 *                      
 */

#pragma once

#include <stdint.h>

struct AppConfig {

    static const uint8_t SM_SENDDATATYPE_GBTPARSED = 0;         // send parsed gbt data
    static const uint8_t SM_SENDDATATYPE_GBTLASTBLOCK = 1;      // send last block of gbt data
    static const uint32_t SM_MEASURE_INTERVAL = 900000;         // default measure interval in ms
    static const uint32_t SM_CYCLE_TIMEOUT = 90000;             // default smartmeter read cycle timeout in ms 
    
    uint32_t smCycleTimeout = SM_CYCLE_TIMEOUT;
    uint8_t sendDataType = SM_SENDDATATYPE_GBTPARSED;
    bool decryptData = false;                                   // decrypt data from smartmeter (currently not fully implemented)
    
    // security byte (1st byte) and authentication key (16 bytes) for smartmeter decryption
    uint8_t authenticationKey[17] = { 0x00, 0xd0, 0xd1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF};

    // aes key and iv for smartmeter decryption
    uint8_t aes_key[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

    // iv for cbc 12 bytes for smartmeter decryption
    uint8_t aes_iv[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    uint32_t appTimer = SM_MEASURE_INTERVAL;	                // measurement intervall (=wakeup timer in ms)

    std::string apiep_hostname = "api.smartspar.ch";            // backend host name for direct connection
    uint16_t apiep_port = 443;                                  // backend port for direct connection
    std::string apiep_url = "/wmb";                             // backend url for post wmb data
    std::string apiep_apikey = "";                              // backend api key for post wmb data
    std::string apiep_certfingerprint = "";                     // backend host fingerprint for tls connection
};