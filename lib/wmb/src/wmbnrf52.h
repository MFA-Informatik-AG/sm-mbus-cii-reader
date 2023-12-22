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
        bool isWlanConnected() override;
        void dataHandler(uint16_t& event_type) override;
        void initApp() override;
        void startWatchDog() override;
        void resetWatchDog() override;
        void delayWithLed(time_t delayWithLedTimeOut) override;
        lmh_error_status enqueueDataPacket(const uint8_t *data, size_t size, uint8_t fport) override;
        bool loadConfiguration(AppConfig& appConfig) override;
        bool saveConfiguration(AppConfig const& appConfig) override;

    private:
        static const int SM_LORA_MAXPAYLOAD = 222;
        static const int SM_LORA_PACKET_DELAY_MS = 5000;
        static const int SM_LORA_PACKET_SIZESTEP = 10;
        static const int SM_LORA_SEND_REPEATER = 10;

        uint16_t m_send_fail = 0;								// counter, lora send fails

        SmCayenne &m_smCayenne;
        AppConfig &m_appConfig;
};

