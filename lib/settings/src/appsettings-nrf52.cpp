/**
 * @file appsettings-nrf52.cpp
 * @brief Implementation of the AppSettings class for NRF52 series.
 * 
 * This file contains the implementation of the AppSettings class for the NRF52 series. 
 * It provides functions for initializing and saving configuration settings to flash memory, 
 * as well as loading and resetting the configuration. The configuration settings include 
 * parameters such as measure interval, send data type, decryption flag, cycle timeout, 
 * AES key, AES initialization vector, and authentication key.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#ifdef NRF52_SERIES

#include <ArduinoJson.h>

#include "appsettings-nrf52.h"
#include "mylog.h"

/**
 * @brief Initializes the flash memory for storing configuration settings.
 * 
 * This function assumes that the MCU flash memory is already initialized by WisBlockApi.
 * It logs a message indicating that the flash access is ready.
 * 
 * @return true if the flash initialization is successful, false otherwise.
 */
bool AppSettings::initFlash()
{
    // assumtpion is that the MCU flash is already initialized by WisBlockApi
    // InternalFS.begin() is executed in WisBlockApi flash initialization
    MyLog::log("APPSETTINGS", "Flash access is ready");

    return true;
}

/**
 * @brief Resets the configuration settings to default values.
 * 
 * This function resets the provided AppConfig object to its default values.
 * It logs a message indicating that the flash configuration is reset to default values.
 * The default values include measure interval, send data type, decryption flag, and cycle timeout.
 * 
 * @param config The AppConfig object to reset.
 */
void AppSettings::resetConfiguration(AppConfig& config)
{
    MyLog::log("APPSETTINGS", "Reset flash configuration to default values");

    config.measureInterval = AppConfig::SM_MEASURE_INTERVAL;
    config.sendDataType = AppConfig::SM_SENDDATATYPE_GBTPARSED;
    config.decryptData = false;
    config.smCycleTimeout = AppConfig::SM_CYCLE_TIMEOUT;

    InternalFS.remove(AppSettings::SETTINGSFILENAME);

    // saveConfiguration(config);
}

/**
 * @brief Saves the configuration settings to flash memory.
 * 
 * This function saves the provided AppConfig object to flash memory.
 * It first initializes the flash memory, then creates a JSON document and populates it with the configuration settings.
 * The JSON document is then serialized and written to the flash memory.
 * It logs a message indicating that the data is saved to flash.
 * 
 * @param config The AppConfig object to save.
 * @return true if the configuration is successfully saved to flash, false otherwise.
 */
bool AppSettings::saveConfiguration(AppConfig const& config) 
{
    if(!initFlash())
    {
        MyLog::log("APPSETTINGS", "Failed to initialize flash");

        return false;
    }

    uint8_t flash_buffer[1024];
    
    StaticJsonDocument<1024> doc;

    MyLog::log("APPSETTINGS", "Save data to flash");

    doc["measureInterval"] = config.measureInterval;
    doc["decryptData"] = config.decryptData;
    doc["sendDataType"] = config.sendDataType;
    doc["smCycleTimeout"] = config.smCycleTimeout;

    JsonArray aeskey = doc.createNestedArray("aes_key");

    for (size_t i = 0; i < sizeof(config.aes_key); i++)
    {
        aeskey.add(config.aes_key[i]);
    }

    JsonArray aesiv = doc.createNestedArray("aes_iv");

    for (size_t i = 0; i < sizeof(config.aes_iv); i++)
    {
        aesiv.add(config.aes_iv[i]);
    }
    
    JsonArray authkey = doc.createNestedArray("authenticationKey");

    for (size_t i = 0; i < sizeof(config.authenticationKey); i++)
    {
        authkey.add(config.authenticationKey[i]);
    }

    serializeJsonPretty(doc, Serial);

    serializeJson(doc, flash_buffer);

    Adafruit_LittleFS_Namespace::File lora_file(InternalFS);

    InternalFS.remove(AppSettings::SETTINGSFILENAME);

    lora_file.open(AppSettings::SETTINGSFILENAME, Adafruit_LittleFS_Namespace::FILE_O_WRITE);
    
    if (!lora_file)
    {
        MyLog::log("APPSETTINGS", "Failed to open file for writing");

        return false;
    }

    lora_file.write(flash_buffer, sizeof(flash_buffer));
    
    lora_file.flush();
    lora_file.close();

    MyLog::log("APPSETTINGS", "Write file successfully");

    return true;
}

/**
 * @brief Loads the configuration settings from flash memory.
 * 
 * This function loads the configuration settings from flash memory into the provided AppConfig object.
 * It first initializes the flash memory, then reads the JSON data from the flash memory.
 * The JSON data is then deserialized and the configuration settings are extracted and stored in the AppConfig object.
 * It logs a message indicating that the data is read from flash.
 * 
 * @param config The AppConfig object to load the configuration settings into.
 * @return true if the configuration is successfully loaded from flash, false otherwise.
 */
bool AppSettings::loadConfiguration(AppConfig& config) 
{
    if(!initFlash())
    {
        MyLog::log("APPSETTINGS", "Failed to initialize flash, using default configuration");

        resetConfiguration(config);

        return false;
    }

    uint8_t flash_buffer[1024];

    MyLog::log("APPSETTINGS", "Read data from flash");

    Adafruit_LittleFS_Namespace::File lora_file(InternalFS);

    lora_file.open(AppSettings::SETTINGSFILENAME, Adafruit_LittleFS_Namespace::FILE_O_READ);
	
    if (!lora_file)
	{
        MyLog::log("APPSETTINGS", "Failed to read file, using default configuration");

        resetConfiguration(config);

        return false;
    }   

    MyLog::log("APPSETTINGS", "Read file successfully");

    lora_file.read(flash_buffer, sizeof(flash_buffer));
    lora_file.close();

    StaticJsonDocument<1024> doc;

    DeserializationError error = deserializeJson(doc, flash_buffer, sizeof(flash_buffer));

    if (error)
    {
        MyLog::log("APPSETTINGS", "Failed to deserialize file, using default configuration");

        resetConfiguration(config);

        return false;
    }

    MyLog::log("APPSETTINGS", "Read file successfully");

    config.measureInterval = doc["measureInterval"];
    config.sendDataType = doc["sendDataType"];
    config.decryptData = doc["decryptData"];
    config.smCycleTimeout = doc["smCycleTimeout"];

    for (size_t i = 0; i < sizeof(config.aes_iv); i++)
    {
        config.aes_iv[i] = doc["aes_iv"][i] | 0;
    }

    for (size_t i = 0; i < sizeof(config.aes_key); i++)
    {
        config.aes_key[i] = doc["aes_key"][i] | 0;
    }
    
    for (size_t i = 0; i < sizeof(config.authenticationKey); i++)
    {
        config.authenticationKey[i] = doc["authenticationKey"][i] | 0;
    }

    return true;
}

#endif