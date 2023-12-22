/**
 * @file appsettings-nrf52.h
 * @brief This file contains the declaration of the AppSettings class for NRF52 series.
 * 
 * This class provides functionality to load, save, and reset the configuration settings
 * for the application using the NRF52 series microcontroller. It uses the InternalFileSystem
 * library for file operations and the AppConfig class for storing the configuration data.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#ifdef NRF52_SERIES
#pragma once

#include <InternalFileSystem.h>

#include "appconfig.h"

class AppSettings
{
    private:
        static constexpr const char* SETTINGSFILENAME = "WMB_SETTINGS";
        static bool initFlash();

    public:
        static bool loadConfiguration(AppConfig& config);
        static bool saveConfiguration(AppConfig const& config);
        static void resetConfiguration(AppConfig& config);        
};
#endif