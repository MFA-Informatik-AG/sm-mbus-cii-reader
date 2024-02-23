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
        static const int SM_LORA_MAXFSIZE = 1024;               // maximum size of the LoRaWan data packet
        static const int SM_LORA_MAXPAYLOAD = 222;
        static const int SM_LORA_PACKET_DELAY_MS = 5000;
        static const int SM_LORA_PACKET_SIZESTEP = 10;
        static const int SM_LORA_SEND_REPEATER = 10;

        uint16_t m_send_fail = 0;			

        uint8_t m_enqueuedDataPacked[SM_LORA_MAXFSIZE];					
	    size_t m_enqueuedDataPackedSize = 0;
        size_t m_enqueuedDataPackedOffset = 0;
        size_t m_enqueuedDataPackedRemainingBytes = 0;
        size_t m_enqueuedDataPackedFport = 0;

        SmCayenne &m_smCayenne;
        AppConfig &m_appConfig;

        lmh_error_status sendEnqueuedData();
};

