/**
 * @file wbmcubase.h
 * @brief This file contains the declaration of the WbMcuBase class.
 * 
 * This class serves as the base class for WisBlock MCU implementations.
 * It provides a set of virtual functions that need to be implemented by derived classes.
 * 
 * @version 1.1.0
 * @author MFA Informatik AG, Andreas Schneider
 * @date 23.01.2024
 * 
 * Version History:
 * 1.1.0    23.01.2024  Renamed isConnectedWLan to connectWlan
 * 
 */
#pragma once

#include <WisBlock-API-V2.h>

class WbMcuBase 
{
    public:
        virtual ~WbMcuBase() = default;                                                                         
        virtual bool connectWlan() = 0;                                                                         // connect or check if the WLAN is connected 
        virtual void dataHandler(uint16_t& event_type) = 0;                                                     // handle data received from the LoRaWAN network
        virtual void initApp() = 0;                                                                             // initialize the MCU
        virtual void startWatchDog() = 0;                                                                       // start the watch dog timer
        virtual void resetWatchDog() = 0;                                                                       // reset the watch dog timer
        virtual void delayWithLed(time_t delayWithLedTimeOut) = 0;                                              // delay with LED indication
        virtual lmh_error_status enqueueDataPacket(const uint8_t *data, size_t size, uint8_t fport) = 0;        // enqueue a data packet to be sent over WAN
        virtual bool loadConfiguration(AppConfig& appConfig);                                                   // load flash stored settings
        virtual bool saveConfiguration(AppConfig const& appConfig);                                             // save settings to flash
        virtual void smDeepSleep() = 0;                                                                         // put the smart meter into deep sleep mode

};