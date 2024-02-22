/**
 * @file Wmbesp32.h
 * @brief This class represents the Wmbesp32 module, which is a subclass of WbMcuBase.
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
#include "appsettings-esp32.h"
#include "wbmcubase.h"

class WmbEsp32 : public WbMcuBase
{
    public:
        WmbEsp32(SmCayenne& smCayenne, AppConfig& appConfig);
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

        uint16_t m_send_fail = 0;								// counter, send fails

        SmCayenne &m_smCayenne;
        AppConfig &m_appConfig;

        String uint8ToHexString(uint8_t *data, size_t length);  // helper function to convert uint8_t array to hex string
};

