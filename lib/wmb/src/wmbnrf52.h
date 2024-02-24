/**
 * @file wmbnrf52.h
 * @brief This class represents the WmbNrf52 module, which is a subclass of WbMcuBase.
 * 
 * It provides functionality for handling data, initializing the application, starting and resetting the watchdog,
 * delaying with LED, and enqueueing data packets. It also contains constants for maximum payload size, packet delay,
 * packet size step, and send repeater.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include "smcayenne.h"
#include "appsettings-nrf52.h"
#include "wbmcubase.h"

class WmbNrf52 : public WbMcuBase
{
    public:
        WmbNrf52(SmCayenne& smCayenne, AppConfig& appConfig);
        bool connectWlan() override;
        void dataHandler(uint16_t& event_type) override;
        void initApp() override;
        void startWatchDog() override;
        void resetWatchDog() override;
        void delayWithLed(time_t delayWithLedTimeOut) override;
        lmh_error_status enqueueDataPacket(const uint8_t *data, size_t size, uint8_t fport) override;
        bool loadConfiguration(AppConfig& appConfig) override;
        bool saveConfiguration(AppConfig const& appConfig) override;
        void smDeepSleep() override;

    private:
        static const int SM_LORA_MAXFSIZE = 1024;                   // maximum size of the LoRaWan data packet buffer
        static const int SM_LORA_MAXPAYLOAD = 111;                  // maximum initial payload size
        static const int SM_LORA_PACKET_DELAY_MS = 8000;            // default delay between sending LoRaWAN packets or if busy
        static const int SM_LORA_PACKET_SIZESTEP = 10;              // decrese step for LoRaWAN payload packet size
        static const int SM_LORA_SEND_REPEATER = 10;                // number of times to repeat sending LoRaWAN packet

        uint16_t m_send_fail = 0;			                        // number of failed LoRaWAN packet sends

        uint8_t m_enqueuedDataBuffer[SM_LORA_MAXFSIZE];			    // buffer for enqueued data		
	    size_t m_enqueuedDataPackedSize = 0;                        // size of enqueued data
        size_t m_enqueuedDataPackedOffset = 0;                      // offset of enqueued data
        size_t m_enqueuedDataPackedRemainingBytes = 0;              // remaining bytes of enqueued data
        size_t m_enqueuedDataPackedFport = 0;                       // fport of enqueued data
        size_t m_enqueuedDataLoraPacketSize =0;                     // size of transmitted LoRaWAN packet

        SmCayenne &m_smCayenne;                                     // reference to SmCayenne object
        AppConfig &m_appConfig;                                     // reference to AppConfig object

        lmh_error_status sendEnqueuedData();                        // send enqueued data over LoRaWAN
};

