/**
 * @file main.cpp
 * @brief Main application file
 *
 * This file contains the main application code for the WisBlock project.
 * It includes various header files and defines global variables and objects.
 * 
 * The setup_app(), init_app(), app_event_handler(), and lora_data_handler() functions are defined here.
 * It also includes callback functions for GBT and HDLC frame handling.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#include <WisBlock-API-V2.h>

#include "appconfig.h"
#include "mylog.h"
#include "smat.h"
#include "wmb.h"
#include "gbt.h"
#include "dlms.h"
#include "hdlc.h"
#include "smlg450.h"
#include "wmbnrf52.h"

#include "main.h"

char g_ble_dev_name[10] = SM_BLE_DEVICENAME;										// device name for BLE (variable name defined and required for wisblock)

AppConfig m_appConfig;																// application config
SmCayenne m_smCayenne(CAYENNEPAYLOAD);												// cayenne data handler		
Gbt m_gbt(gbt_frame_handler);    													// gbt protocol handler
Dlms m_dlms(m_gbt);																	// sort of dlsm protocol handler but only used for gbt
Hdlc m_hdlc(hdlc_frame_handler);													// m_hdlc protocol handler
SmLg450 m_lg450;																	// smartmeter handler

#ifdef NRF52_SERIES
WmbNrf52 m_wmbnrf52(m_smCayenne, m_appConfig);										// wisblock nr52 mcu
WbMcuBase& m_wmb = m_wmbnrf52;													
#endif

#ifdef ARDUINO_ARCH_ESP32
//todo: add esp32 mcu
#endif

Wmb wmb(m_wmb, m_lg450, m_gbt, m_dlms, m_hdlc, m_smCayenne, m_appConfig);			// wmb controller 

/**
 * 
 * @brief	Setup function called from the WisBlock framework
 * 
 * Called once at startup from the WisBlock framework. This function is used to setup the application.
 * 
 * 
*/
void setup_app()
{

#if MY_DEBUG > 0

	Serial.begin(115200);

	time_t serial_timeout = millis();

	// On nRF52840 the USB serial is not available immediately
	// however delay is helpfull also for other boards
	while (!Serial)
	{
		if ((millis() - serial_timeout) < SM_RF52USB_SERIAL_DELAY)
		{
			delay(100);
			
			digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
		}
		else
		{
			break;
		}
	}

  	digitalWrite(LED_GREEN, LOW);

#endif

#if MY_BLE_DEBUG > 0
	
	g_enable_ble=true;	// enable ble (required for wisblock)	 	 

#else

	g_enable_ble=false;	// disable ble (required for wisblock)	 

#endif

	// Read credentials from wisblock
	api_read_credentials();

	// store modified credentials
	api_set_credentials();

	// set version
  	api_set_version(SW_VERSION_1, SW_VERSION_2, SW_VERSION_3);

	// setup the wmb adapter
	wmb.setupApp();
}

/**
 * 
 * @brief Init function called from the WisBlock framework
 * 
 * Initializes the application. This function is called from the WisBlock framework after the setup function.
 * It must return true if the application is successfully initialized.
 * 
 * @return true if the application is successfully initialized
 * @return false if the application is not successfully initialized
 * 
 * 
*/
bool init_app()
{
	MyLog::log("APP", "Init app called");

	wmb.initApp();

	// now the timer can be set
	api_timer_restart(m_appConfig.g_appTimer);

	MyLog::log("APP", "..set LoRaWAN timer (wakeup from deep sleep) to %d ms", m_appConfig.g_appTimer);

	// initializes smartmeter AT commands
	smcustom_at_init();

	MyLog::log("APP", "Init app completed");

	return true;
}


/**
 * 
 * @brief Application status event handler called from the WisBlock framework
 * 
 * This function is called from the WisBlock framework if an application status event occurs.
 * 
 * 
*/
void app_event_handler()
{
	// Timer triggered events
	if ((g_task_event_type & STATUS) == STATUS)
	{		
		// flag must be reset (required for wisblock)
		g_task_event_type &= N_STATUS;

		MyLog::log("APP", "App event handler timer wakeup");
		
		wmb.smReadSendcycle();
	}

	// AT triggered events
	if((g_task_event_type & AT_SM_READSENDCYCLE) == AT_SM_READSENDCYCLE)
	{
		// flag must be reset (required for wisblock)
		g_task_event_type &= AT_N_SM_READSENDCYCLE;

		MyLog::log("APP", "App event handler AT read cycle");
		
		wmb.smReadSendcycle();
	}
}


/**
 * 
 * @brief LoRaWAN data event handler called from the WisBlock framework
 * 
 * 
*/
void lora_data_handler()
{
	wmb.dataHandler(g_task_event_type);
}


/**
 * @brief Handles the GBT frame.
 * 
 * This function is responsible for handling the GBT frame by passing the data and size to the `wmb.gbtFrameHandler` function.
 * 
 * @param data The GBT frame data.
 * @param size The size of the GBT frame data.
 * 
 */
void gbt_frame_handler(uint8_t const* data, size_t const size) 
{
	wmb.gbtFrameHandler(data, size);
}

/**
 * @brief Handles HDLC frame data.
 * 
 * This function is responsible for processing HDLC frame data.
 * It takes a pointer to the data, the size of the data, and a flag indicating if the data is valid.
 * 
 * @param data Pointer to the HDLC frame data.
 * @param size Size of the HDLC frame data.
 * @param valid Flag indicating if the data is valid.
 *
 */
void hdlc_frame_handler(const uint8_t *data, size_t const size, bool const valid) 
{
	wmb.hdlcFrameHandler(data, size, valid);
}

    