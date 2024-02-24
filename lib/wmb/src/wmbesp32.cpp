/**
 * @file wmbesp32.cpp
 * @brief Implementation file for Wmbesp32 class
 * 
 * This file contains the implementation of the Wmbesp32 class, which is responsible for managing the Wmbesp32 module.
 * The class provides methods for loading and saving configuration, controlling the watchdog, initializing the application,
 * checking WLAN connectivity, handling data events, and sending LoRaWAN packets.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#ifdef ESP32

#include "WiFiClientSecure.h"
#include "string.h"

#include "appsettings-esp32.h"
#include "esp_task_wdt.h"
#include "mylog.h"
#include "wmbesp32.h"

/**
 * 
 * @brief Constructor
 * 
 * @param smCayenne Reference to SmCayenne object
 * @param appConfig Reference to AppConfig object
 * 
*/
/**
 * 
 * @brief Constructor
 * 
 * @param smCayenne Reference to SmCayenne object
 * @param appConfig Reference to AppConfig object
 * 
*/
WmbEsp32::WmbEsp32(SmCayenne &smCayenne, AppConfig &appConfig) :
	m_smCayenne(smCayenne), 
	m_appConfig(appConfig)
{
}


/**
 * Loads the configuration for the WmbNrf52 module.
 * 
 * @param appConfig The AppConfig object to load the configuration into.
 * @return True if the configuration was successfully loaded, false otherwise.
 */
bool WmbEsp32::loadConfiguration(AppConfig& appConfig)
{
	return AppSettings::loadConfiguration(appConfig);
}


/**
 * Saves the configuration of the WmbNrf52 device.
 * 
 * @param appConfig The application configuration to be saved.
 * @return True if the configuration was successfully saved, false otherwise.
 */
bool WmbEsp32::saveConfiguration(AppConfig const& appConfig)
{
	return AppSettings::saveConfiguration(appConfig);
}


/**
 * 
 * @brief Delay with LED helper
 * 
 * This helper is used to signal that something is going on. The LED is turned on and off during the delay.
 * 
*/
void WmbNrf52::delayWithLed(time_t delayWithLedTimeOut)
{
	time_t timeout = millis();

	while ((millis() - timeout) < delayWithLedTimeOut)
	{
		digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

		delay(100);
	}
			
	digitalWrite(LED_BUILTIN, LOW);
}

/**
 * 
 * @brief Set the Watch Dog object
 * 
 * Enables the watchdog and sets the timeout
*/
void WmbEsp32::startWatchDog()
{
	uint32_t timeout = m_appConfig.appTimer * 3;

	MyLog::log("ESP32", "Start Watchdog with %d sec.", timeout);

	// https://iotassistant.io/esp32/enable-hardware-watchdog-timer-esp32-arduino-ide/
	esp_task_wdt_init(timeout, true);
}

/**
 * 
 * @brief Reset the Watch Dog object
 * 
 * Resets the watchdog so the timer starts from the beginning. This function must be called within the timeout period.
 * Otherwise the device will be reset.
 * 
*/
void WmbEsp32::resetWatchDog()
{
	MyLog::log("ESP32", "Reset Watchdog");

	esp_task_wdt_reset();
}

void WmbEsp32::smDeepSleep()
{
	MyLog::log("ESP32", "Set MCU into deep sleep mode");

	esp_deep_sleep_start();
}


/**
 * 
 * @brief Initialize the application
 * 
 * This function is used to initialize the application. It sets the initial repeat time and the confirmed message
 * enabled flag.
 * 
*/
void WmbEsp32::initApp(void)
{
    // set the initial repeat time, must be set otherwise there is an issue with the wisblock api
	// we keep the g_lorawan_settings also if we use WLAN (for now)
	// todo: check if the name should be refactored to be wlan agnostic
    g_lorawan_settings.send_repeat_time = m_appConfig.appTimer;
	
	MyLog::log("ESP32", "Init WLAN");

	// creates the unique device name based on the esp32 mac address
	// required also to xor ble data
	create_dev_name();

	MyLog::log("ESP32", "Device name set to %s", g_ap_name);

	// esp32 specific deep sleep
	uint32_t sleepTime = m_appConfig.appTimer * 1000;

	MyLog::log("ESP32", "Init deep sleep wakeup timer %d us", sleepTime);

	esp_sleep_enable_timer_wakeup(sleepTime);
}

/**
 * 
 * @brief Connect/check if WLAN is connected
 * 
 * This function is used to connect/check if WLAN is connected.
 * 
 * @return true if WLAN is connected
 * @return false if WLAN is not connected
*/
bool WmbEsp32::connectWlan()
{
	if(!g_wifi_connected)
	{
		g_ssid_prim = "";
		g_pw_prim = "";
		g_ssid_sec = "";
		g_pw_sec = "";

		init_wifi();
	}

    return g_wifi_connected;
}

/**
 * 
 * @brief Data handler
 * 
 * This function is used to handle data. It handles LoRa data received, LoRa TX finished, and LoRa join finished.
 * 
 * @param event_type Event type
 * 
*/
void WmbEsp32::dataHandler(uint16_t& event_type)
{
	// the function is currently not used for ESP32	
}


/**
 * 
 * @brief Send WAN packet helper
 * 
 * This helper is used to send a WAN packet. 
 * If the WAN  is busy the sending is repeated after a delay.
 * 
*/
lmh_error_status WmbEsp32::enqueueDataPacket(const uint8_t *data, size_t size, uint8_t fport)
{
	MyLog::log("ESP32", "WiFi send %d bytes to port %d", size, fport);

	MyLog::logHex("ESP32", "..enqueue data packet: ", data, size);

	uint8_t repeatBusy = 0;
	uint8_t errorCount = 0;

	if(!g_wifi_connected)
	{
		MyLog::log("ESP32", "WiFi not connected");

		return lmh_error_status::LMH_ERROR;
	}

	WiFiClientSecure client;	

	bool hasFingerprint = m_appConfig.apiep_certfingerprint.length() > 0;

	if(!hasFingerprint)
	{
		client.setInsecure();
	}

	if (!client.connect(m_appConfig.apiep_hostname.c_str(), m_appConfig.apiep_port))
	{
		MyLog::log("ESP32", "Connection failed");

		return lmh_error_status::LMH_ERROR;
	}

	if (hasFingerprint && !client.verify(m_appConfig.apiep_certfingerprint.c_str(), m_appConfig.apiep_hostname.c_str())) 
	{
		MyLog::log("ESP32", "Certificate mismatch");

		return lmh_error_status::LMH_ERROR;
	}

	MyLog::log("ESP32", "connected to server %s", m_appConfig.apiep_hostname.c_str());

	String sendData = "{'key1': 'value1',";  
	sendData += "'key2':'" + uint8ToHexString((uint8_t*) data, size) + "'}";

	// Create the HTTP request
	String postRequest = "POST " + String(m_appConfig.apiep_url.c_str()) + " HTTP/1.1\r\n";
	postRequest += "Host: " + String(m_appConfig.apiep_hostname.c_str()) + "\r\n";
	postRequest += "Content-Type: application/json\r\n";
	postRequest += "Content-Length: " + String(sendData.length()) + "\r\n";
	postRequest += "Connection: close\r\n\r\n";
	postRequest += sendData;

	MyLog::log("ESP32", "Sending POST request: %s", postRequest.c_str());

	// Send the POST request
	client.print(postRequest);

	time_t client_timeout = millis();

	while (client.connected()) {

		if ((millis() - client_timeout) < 15000)
		{
			String line = client.readStringUntil('\r');

			MyLog::log("ESP32", "Response: %s", line.c_str());
		}
		else
		{
			break;
		}
	}

	return lmh_error_status::LMH_SUCCESS;
}

String WmbEsp32::uint8ToHexString(uint8_t *data, size_t length) 
{
	String hexString = "";
    
	for (size_t i = 0; i < length; i++) {
    
        if (data[i] < 0x10) 
		{
			hexString += "0"; 
		}
        
		hexString += String(data[i], HEX);
    }
    
	hexString.toUpperCase(); 
    
	return hexString;
}

#endif
