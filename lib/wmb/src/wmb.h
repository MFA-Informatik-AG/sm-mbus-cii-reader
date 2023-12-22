/**
 * @file wmb.h
 * @brief Header file for the Wmb class.
 * 
 * This file contains the declaration of the Wmb class, which represents a WisBlock module.
 * The Wmb class provides functionality for initializing the module, setting up the application,
 * reading and sending data, handling events, and managing various protocols.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <WisBlock-API-V2.h>

#include "smcayenne.h"
#include "appconfig.h"
#include "gbt.h"
#include "dlms.h"
#include "hdlc.h"
#include "smbase.h"    
#include "wbmcubase.h"

class Wmb 
{
    public:
        Wmb(WbMcuBase& wbMcu, SmBase& smartmeter, Gbt& gbt, Dlms& dlms, Hdlc& hdlc, SmCayenne& smCayenne, AppConfig& appConfig);    
        bool initApp();                                                                                                             // initialize the application                            
        void setupApp();                                                                                                            // setup the application                    
        void smReadSendcycle();                                                                                                     // read and send data from the smart meter
        void dataHandler(uint16_t event_type);                                                                                      // handle data received from the WAN                  
        void gbtFrameHandler(uint8_t const* data, size_t const size);                                                               // handle GBT frames received from the smart meter
        void hdlcFrameHandler(uint8_t const* data, size_t const size, bool const valid);                                            // handle HDLC frames received from the smart meter

    private:
        static const int SM_GBT_MAXFRAMESIZE = 1024;            // maximum size of the GBT frame

        uint32_t g_appTimer = AppConfig::SM_MEASURE_INTERVAL;	// measurement intervall (=wakeup timer in ms)
        uint16_t m_send_fail = 0;								// counter, WAN send fails
        uint32_t m_sm_readLoops = 0;							// counter, total of read loops

        uint8_t m_lastGbtFrameReceived[SM_GBT_MAXFRAMESIZE];	// last gbt frame received from smartmeter
        size_t m_lastGbtFrameReceivedSize;						// last gbt frame received length

        WbMcuBase& m_wbMcu;                                     // wisblock mcu
        SmBase& m_smartmeter;                                   // smartmeter handler
        Gbt &m_gbt;                                             // gbt protocol handler
        Dlms &m_dlms;								            // dlsm protocol handler
        Hdlc &m_hdlc;                                           // hdlc protocol handler
        SmCayenne& m_smCayenne;                                 // cayenne data handler
        AppConfig& m_appConfig;									// application settings handler

        void wmbadaper_addStates(SmCayenne& cayenne);              // stores adapter states to cayenne
        void wmbadaper_battery(SmCayenne& cayenne);             // stores battery status to cayenne
        void wmbadapter_addSendReadLoopsCounter(SmCayenne& cayenne);          // stores send loops to cayenne
        void wmbadapter_addSendFailuresCounter(SmCayenne& cayenne);           // stores send fails counter to cayenne
        void save_appConfig(AppConfig& appConfig);              // stores application settings to flash

        void mbusAdapterOff();                                  // switch off mbus adapter
        void mbusAdapterOn();                                   // switch on mbus adapter

        void smReadcycle();                                     // read data from the smart meter
};
