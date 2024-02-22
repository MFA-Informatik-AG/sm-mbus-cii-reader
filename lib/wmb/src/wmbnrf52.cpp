/**
 * @file wmbnrf52.cpp
 * @brief Implementation file for WmbNrf52 class
 * 
 * This file contains the implementation of the WmbNrf52 class, which is responsible for managing the WmbNrf52 module.
 * The class provides methods for loading and saving configuration, controlling the watchdog, initializing the application,
 * checking WLAN connectivity, handling data events, and sending LoRaWAN packets.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#ifdef NRF52_SERIES

#include "mylog.h"
#include "wmbnrf52.h"
#include "appsettings-nrf52.h"

/**
 * 
 * @brief Constructor
 * 
 * @param smCayenne Reference to SmCayenne object
 * @param appConfig Reference to AppConfig object
 * 
*/
WmbNrf52::WmbNrf52(SmCayenne &smCayenne, AppConfig &appConfig) : 
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
bool WmbNrf52::loadConfiguration(AppConfig& appConfig)
{
	return AppSettings::loadConfiguration(appConfig);
}


/**
 * Saves the configuration of the WmbNrf52 device.
 * 
 * @param appConfig The application configuration to be saved.
 * @return True if the configuration was successfully saved, false otherwise.
 */
bool WmbNrf52::saveConfiguration(AppConfig const& appConfig)
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

	while (!Serial)
	{
		if ((millis() - timeout) < delayWithLedTimeOut)
		{
			digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
		}
		else
		{
			break;
		}
	}
			
	digitalWrite(LED_BUILTIN, LOW);
}

/**
 * 
 * @brief Set the Watch Dog object
 * 
 * Enables the watchdog and sets the timeout
*/
void WmbNrf52::startWatchDog()
{
	MyLog::log("NRF52", "Start Watchdog");

	NRF_WDT->CONFIG         = 0x01;     							            // Configure WDT to run when CPU is asleep
	NRF_WDT->CRV			= ((m_appConfig.appTimer / 1000) * 3) * 32768;		// Timeout (*3 for margin) to App Timer (value in sec * 32768)
	NRF_WDT->RREN           = 0x01;     							            // Enable the RR[0] reload register
	NRF_WDT->TASKS_START    = 1;        							            // Start WDT       
}

/**
 * 
 * @brief Reset the Watch Dog object
 * 
 * Resets the watchdog so the timer starts from the beginning. This function must be called within the timeout period.
 * Otherwise the device will be reset.
 * 
*/
void WmbNrf52::resetWatchDog()
{
	MyLog::log("NRF52", "Reset Watchdog");

	NRF_WDT->RR[0] = WDT_RR_RR_Reload; 
}

/**
 * 
 * @brief Deep sleep
 * 
 * This function is used to put the device into deep sleep mode.
 * 
*/

void WmbNrf52::smDeepSleep()
{
	// not implemented, handled from the WisBlock framework

}


/**
 * 
 * @brief Initialize the application
 * 
 * This function is used to initialize the application. It sets the initial repeat time and the confirmed message
 * enabled flag.
 * 
*/
void WmbNrf52::initApp(void)
{
    // set the initial repeat time, must be set otherwise there is an issue with the wisblock api
    g_lorawan_settings.send_repeat_time = m_appConfig.appTimer;
    g_lorawan_settings.confirmed_msg_enabled = LMH_CONFIRMED_MSG;
}

/**
 * 
 * @brief Connect/Check if WLAN is connected
 * 
 * This function is used to connect/check if WLAN is connected.
 * 
 * @return true if WLAN is connected
 * @return false if WLAN is not connected
*/
bool WmbNrf52::connectWlan()
{
    lmh_join_status join_status = lmh_join_status_get();

    return join_status == lmh_join_status::LMH_SET;
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
void WmbNrf52::dataHandler(uint16_t& event_type)
{
	// LoRa data receved handling
	if ((event_type & LORA_DATA) == LORA_DATA)
	{
		// flag must be reset (required for wisblock)
		event_type &= N_LORA_DATA;

		MyLog::logHex("NRF52", "LoRaWAN RX data packet: ", g_rx_lora_data, g_rx_data_len);

		SmCayenne smReceived(g_rx_data_len);

        // store the settings into the current configuration
		smReceived.smDecodeReceivedAppSettings(g_rx_lora_data, g_rx_data_len, m_appConfig);

		// restart the board
		api_reset();
	}

	// LoRa TX finished handling
	if ((g_task_event_type & LORA_TX_FIN) == LORA_TX_FIN)
	{
		// flag must be reset (required for wisblock)
		g_task_event_type &= N_LORA_TX_FIN;

		MyLog::log("NRF52", "LoRaWAN  TX cycle %s", g_rx_fin_result ? "finished ACK" : "failed NAK");

		if (g_rx_fin_result)
		{
			// reset fail send counter
			m_send_fail = 0;
		}
		else
		{
			// increase fail send counter
			m_send_fail++;

			MyLog::log("NRF52", "LoRaWAN failed NAK counter %d", m_send_fail);

			if (m_send_fail < SM_LORA_SEND_REPEATER)
			{
				if(m_smCayenne.getSize() > 0)
				{
					MyLog::log("NRF52", "LoRaWAN failed sending, repeat enqueue last packet");

					enqueueDataPacket(m_smCayenne.getBuffer(), m_smCayenne.getSize(), 0);
				}
			}
			else
			{
				MyLog::log("NRF52", "LoRaWAN to many failed sendings, reset node");

				api_reset();
			}
		}

		resetWatchDog();
	}

	// LoRa Join finished handling
	if ((g_task_event_type & LORA_JOIN_FIN) == LORA_JOIN_FIN)
	{
        //
		// flag must be reset (required for wisblock)
        //
		g_task_event_type &= N_LORA_JOIN_FIN;

		if (g_join_result)
		{
			MyLog::log("NRF52", "LoRaWAN successfully joined network");
		}
		else
		{
			MyLog::log("NRF52", "LoRaWAN join network failed. Request rejoin...");

			lmh_join();
		}

		resetWatchDog();
	}
}


/**
 * 
 * @brief Send LoRaWAN packet helper
 * 
 * This helper is used to send a LoRaWAN packet. The packet is sent in chunks if required. The chunks are
 * sent one after the other. If the LoRaWAN transceiver is busy the sending is repeated after a delay.
 * 
*/
lmh_error_status WmbNrf52::enqueueDataPacket(const uint8_t *data, size_t size, uint8_t fport)
{
	MyLog::log("NRF52", "LoRaWAN send %d bytes to port %d", size, fport);

	MyLog::logHex("NRF52", "..enqueue Lora packet: ", data, size);

	// https://lora-developers.semtech.com/documentation/tech-papers-and-guides/the-book/packet-size-considerations
	size_t loraPayloadSize = min((size_t) SM_LORA_MAXPAYLOAD, size); 
	size_t remainingBytes = size;
	size_t offset = 0;
	uint8_t repeatBusy = 0;
	uint8_t errorCount = 0;

	while(offset < size)
	{
		size_t loraPacketSize = min(loraPayloadSize, remainingBytes);

		MyLog::log("NRF52", ".. queue Lora packet, maxLoraPayload %d, remainingBytes %d", loraPayloadSize, remainingBytes);

		uint8_t sendData[loraPacketSize];

		memcpy(sendData, data + offset, loraPacketSize);

		lmh_error_status result = send_lora_packet(sendData, loraPacketSize, fport);

		MyLog::log("NRF52", ".. Lora packet, offset %d, size %d, repeatBusy %d", offset, loraPacketSize, repeatBusy);

		switch (result)
		{
			case LMH_SUCCESS:
				MyLog::log("NRF52", "LoRaWAN packet enqueued");
				offset += loraPacketSize;
				remainingBytes -= loraPacketSize;
				repeatBusy = 0;
				break;
			case LMH_BUSY:
				MyLog::log("NRF52", "LoRaWAN transceiver is busy, repeat send");
				delay(SM_LORA_PACKET_DELAY_MS);
				repeatBusy++;
				break;
			case LMH_ERROR:
				errorCount++;
				MyLog::log("NRF52", "LoRaWAN packet error count %d, maybe too big with %d bytes to send with current datarate", errorCount, loraPayloadSize);
				if(errorCount > 2)
				{
					loraPayloadSize -= SM_LORA_PACKET_SIZESTEP;

					MyLog::log("NRF52", "..try to reduce packet size to %d bytes", loraPayloadSize);
				}
				delay(SM_LORA_PACKET_DELAY_MS);
				break;
		}

		if(loraPayloadSize < SM_LORA_PACKET_SIZESTEP)
		{
			MyLog::log("NRF52", "LoRaWAN packet size could not be determined");

			return lmh_error_status::LMH_ERROR;
		}	

		if(repeatBusy > SM_LORA_SEND_REPEATER)
		{
			MyLog::log("NRF52", "LoRaWAN packet could not be send after %d retries", repeatBusy);

			return lmh_error_status::LMH_ERROR;
		}
	}

	return lmh_error_status::LMH_SUCCESS;
}

#endif
