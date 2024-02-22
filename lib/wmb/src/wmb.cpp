/**
 * @file wmb.h
 * @brief Header file for the Wmb class.
 * 
 * This file contains the implementation of the Wmb class, which is responsible for handling the communication
 * between the WisBlock MCU, smart meter, GBT, DLMS, HDLC, and Cayenne. It provides functions for initializing
 * the application, handling data events, reading and sending cycles, and managing the M-BUS adapter. The class
 * also includes functions for handling GBT frames and HDLC frames, as well as saving the application configuration.
 *
 * The Wmb class is designed to be used in conjunction with other classes and libraries to create a complete
 * application for monitoring and controlling smart meters.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 * 
 */
#include "gbtdata.h"					
#include "mylog.h"						
#include "wmb.h"


/**
 * @brief Constructor for the Wmb class.
 * 
 * @param wbMcu Reference to the WbMcuBase object.
 * @param smartmeter Reference to the SmBase object.
 * @param gbt Reference to the Gbt object.
 * @param dlms Reference to the Dlms object.
 * @param hdlc Reference to the Hdlc object.
 * @param smCayenne Reference to the SmCayenne object.
 * @param appConfig Reference to the AppConfig object.
 */
Wmb::Wmb(WbMcuBase& wbMcu, SmBase& smartmeter, Gbt& gbt, Dlms& dlms, Hdlc& Hdlc, SmCayenne &smCayenne, AppConfig &appConfig) : 
     m_wbMcu(wbMcu), 
	 m_smartmeter(smartmeter),
	 m_gbt(gbt),
	 m_dlms(dlms),
	 m_hdlc(Hdlc),
     m_smCayenne(smCayenne), 
     m_appConfig(appConfig)
{
}

/**
 * @brief Handles the data for the Wmb class.
 *
 * This function is responsible for handling the data based on the event type (forwards it to the MCU)
 *
 * @param event_type The type of event to be handled.
 */
void Wmb::dataHandler(uint16_t event_type)
{
    m_wbMcu.dataHandler(event_type);
}

/**
 * @brief Sets up the Wmb application.
 */
void Wmb::setupApp()
{
     m_wbMcu.startWatchDog();
} 


/**
 * @brief Puts the smart meter into deep sleep mode.
 * 
 * This function puts the smart meter into deep sleep mode.
 */
void Wmb::smDeepSleep()
{
	m_wbMcu.smDeepSleep();
}

/**
 * @brief Initializes the WMB application.
 * 
 * This function initializes the WMB application 
 * 
 * @return true if the initialization is successful, false otherwise.
 */
bool Wmb::initApp()
{
    mbusAdapterOff();

	MyLog::log("WMB", "Load application config");

	m_wbMcu.loadConfiguration(m_appConfig);

	MyLog::log("APPSETTINGS", "...appTimer: %d", m_appConfig.appTimer);
    MyLog::log("APPSETTINGS", "...sendDataType: %d", m_appConfig.sendDataType);
    MyLog::log("APPSETTINGS", "...decryptData: %d", m_appConfig.decryptData);
    MyLog::log("APPSETTINGS", "...smCycleTimeout: %d", m_appConfig.smCycleTimeout);

	if(m_appConfig.decryptData)
	{
		MyLog::log("APPSETTINGS", "...sizeof aes_iv: %d", sizeof(m_appConfig.aes_iv));
		MyLog::log("APPSETTINGS", "...sizeof aes_key: %d", sizeof(m_appConfig.aes_key));
		MyLog::log("APPSETTINGS", "...sizeof authenticationKey: %d", sizeof(m_appConfig.authenticationKey));
	}

	MyLog::log("WMB", "...reading global values for WisBlock timer");

    m_wbMcu.initApp();

    return true;
}

/**
 * @brief Reads and sends data from the smart meter.
 * 
 * This function reads and sends data from the smart meter.
 */
void Wmb::smReadSendcycle()
{
	MyLog::log("WMB", "Execute Smartmeter read and send cycle");

	time_t readSendCycleTimeStart = millis();

	smReadcycle();

	// adds the adapter states into the cayenne buffer
	wmbadaper_addStates(m_smCayenne);

	if(!m_wbMcu.connectWlan())
	{
		MyLog::log("WMB", "WLAN not connected, skip sending");

		return;
	}

	// application setting how to send the data via WAN
	uint8_t sendDataType = m_appConfig.sendDataType;

	// default way via Cayenne LPP (decoded)
	if(sendDataType == AppConfig::SM_SENDDATATYPE_GBTPARSED)
	{
		uint8_t gbtSize = m_smCayenne.getSize();

		if(gbtSize > 0)
		{
			MyLog::log("WMB", "...send %d bytes via WAN", gbtSize);

			uint8_t cayenneError = m_smCayenne.getError();

			if(cayenneError == 0)
			{
				m_wbMcu.enqueueDataPacket(m_smCayenne.getBuffer(), gbtSize, 0);
			}
			else	
			{
				MyLog::log("WMB", "...send GBT packed aborted with Cayenne LPP error %d", cayenneError);
			}
		}
		else
		{
			MyLog::log("WMB", "...no GBT packet to send (size 0)");
		}
	}
	// raw way, send the GBT block as is, decoding has to be done on the server side
	else if(sendDataType == AppConfig::SM_SENDDATATYPE_GBTLASTBLOCK)
	{
		MyLog::log("WMB", "...send full last GBT block via WAN");

		m_wbMcu.enqueueDataPacket(m_lastGbtFrameReceived, m_lastGbtFrameReceivedSize, 0);
	}

	time_t readSendCycleTimeRun = millis() - readSendCycleTimeStart;

	MyLog::log("WMB", "SM read and send cycle completed in %ld ms", readSendCycleTimeRun);

    m_wbMcu.resetWatchDog();
}

/**
 * @brief Switches off the M-BUS adapter.
 */
void Wmb::mbusAdapterOff()
{
    MyLog::log("WMB", "...enable M-BUS adapter");

	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, LOW);
}

/**
 * @brief Switches on the M-BUS adapter.
 */
void Wmb::mbusAdapterOn()
{
    MyLog::log("WMB", "...enable M-BUS adapter");

	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
}

/**
 * @brief Handles the GBT frame received from the smart meter.
 * 
 * This function handles the GBT frame received from the smart meter.
 * 
 * @param data Pointer to the GBT frame data.
 * @param size The size of the GBT frame data.
 */
void Wmb::gbtFrameHandler(uint8_t const* data, size_t const size) 
{
	// store the last gbt frame received
	if(size > SM_GBT_MAXFRAMESIZE)
	{
		MyLog::log("WMB", "GBT frame received with %d bytes, max size is %d, frame dropped", size, SM_GBT_MAXFRAMESIZE);

		return;
	}

	// save the last received gbt frame
	memcpy(m_lastGbtFrameReceived, data, size);

	m_lastGbtFrameReceivedSize = size;

	MyLog::log("WMB", "GBT frame received");

    GbtData gbtData;

	MyLog::log("WMB", "GBT frame parse data");

    if(gbtData.parse(data, size) == 0)
	{
		MyLog::log("WMB", "GBT frame parse %d block of data successfull", size);
	}
	else
    {
		MyLog::log("WMB", "GBT frame parse failed");	
    }

	MyLog::log("WMB", "GBT allow process smartmeter specifics   cayenne");

	// additional and specific smart meter data to be parsed 
	m_smartmeter.parse(gbtData);

	MyLog::log("WMB", "GBT copy block into cayenne");

	m_smartmeter.copyData(gbtData, m_smCayenne);

	if(m_smCayenne.getError() == LPP_ERROR_OK)
	{
		MyLog::log("WMB", "GBT copy into cayenne sucessfull");
	}
	else
	{
		MyLog::log("WMB", "GBT copy into cayenne failed");
	}	
}

/**
 * @brief Handles the HDLC frame received from the smart meter.
 * 
 * This function handles the HDLC frame received from the smart meter.
 * 
 * @param data Pointer to the HDLC frame data.
 * @param size The size of the HDLC frame data.
 * @param valid Indicates whether the HDLC frame is valid.
 */
void Wmb::hdlcFrameHandler(const uint8_t *data, size_t const size, bool const valid) 
{
	if(!valid)
	{
		MyLog::logHex("WMB", "Invalid frame content received with: ", data, size);

		MyLog::log("WMB", "Reset dlsm receive buffer");

		m_dlms.reset();

		return;
	}

	MyLog::log("WMB", "Valid frame received with %d bytes", size);

	MyLog::logHex("WMB", "Frame content: ", data, size);

	// remove the HDLC header and footer
    uint8_t hdlcData[size];
    size_t hdlcDataSize = size - 8;

    memcpy(hdlcData, data + 8, hdlcDataSize);

    GbtBlock const* addedBlock;

	MyLog::log("WMB", "Parse HDLC frame content");

    if(m_dlms.hdlcDataReceived(hdlcData, hdlcDataSize, addedBlock))
    {
		MyLog::log("WMB", "Frame content detected as GBT, add GBT frame block");
    }
    else
    {
		MyLog::log("WMB", "Frame could not be detected, frame dropped");
    }
}

/**
 * @brief Reads data from the smart meter.
 * 
 * This function reads data from the smart meter.
 */
void Wmb::smReadcycle()
{
	m_sm_readLoops += 1;

	MyLog::log("WMB", "Start read cycle %u", m_sm_readLoops);

	MyLog::log("WMB", "...turn on wired m-bus adapter");

	// turn the m-bus slave card on
	mbusAdapterOn();

	// some time to warm-up and signal that something is going on
    m_wbMcu.delayWithLed(500);

	MyLog::log("WMB", "...wired m-bus adapter enabled");
	MyLog::log("WMB", "...open serial port for smartmeter reading");

	m_smartmeter.openSerialPort();

	// some time to warm-up and signal that something is going on
	m_wbMcu.delayWithLed(500);

	MyLog::log("WMB", "...serial port opened");

	MyLog::log("WMB", "...reset the HDLC protocol handler");

	// reset the m_dlms receive buffer
	m_dlms.reset();

	MyLog::log("WMB", "...HDLC protocol handler reset");

	MyLog::log("WMB", "...set the receive cycle with timeout %u", m_appConfig.smCycleTimeout);

	time_t cycleTimeOut = m_appConfig.smCycleTimeout;

	MyLog::log("WMB", "...start the receive cycle with timeout %u", cycleTimeOut);

	time_t readCycleStartTime = millis();

	// start the receive cycle for the smart meter data
	while(!m_dlms.gbtFrameReceived())
	{
		if((millis() - readCycleStartTime) > cycleTimeOut)
		{
			MyLog::log("WMB", "...read cycle timeout of %d reached, breaking read cycle", cycleTimeOut);

			break;
		}

		if(m_smartmeter.available() > 0)
		{
			// read the next byte from the smart meter
			int number = m_smartmeter.read();

			if(number >=0)
			{
				int newState = !digitalRead(LED_BUILTIN);   

				digitalWrite(LED_BUILTIN, newState);

				// enqueue the byte into the HDLC protocol handler
				m_hdlc.charReceiver((uint8_t) number);
			}
		}
		else
		{
			digitalWrite(LED_BUILTIN, LOW);

			delay(100);
		}
	}


	MyLog::log("WMB", "...read cycle completed");

	MyLog::log("WMB", "...close serial port");

	m_smartmeter.closeSerialPort();

	MyLog::log("WMB", "...serial port closed");

	MyLog::log("WMB", "...turn off wired m-bus adapter");

	mbusAdapterOff();

	m_wbMcu.delayWithLed(500);

	MyLog::log("WMB", "...wired m-bus adapter disabled");

	MyLog::log("WMB", "Read cycle completed");
}

/**
 * @brief Saves the application configuration.
 * 
 * This function saves the application configuration.
 * 
 * @param appConfig Reference to the AppConfig object.
 */
void Wmb::save_appConfig(AppConfig& appConfig)
{
	m_wbMcu.saveConfiguration(appConfig);
}

/**
 * @brief Stores the battery status to Cayenne.
 * 
 * This function stores the battery status to Cayenne. It calculates the average battery voltage
 * 
 * @param cayenne Reference to the SmCayenne object.
 */
void Wmb::wmbadaper_battery(SmCayenne& cayenne)
{
	const uint8_t READBATTVALUELOOP = 25;

	float read_val = 0.0;

	for (int i = 0; i < READBATTVALUELOOP; i++)
	{
		read_val += read_batt();
	}
	
	auto battstate = (uint16_t) (read_val / READBATTVALUELOOP);

	MyLog::log("WMB", "..add battery voltage %d", battstate);

	cayenne.addBatteryVoltage(m_smartmeter.getChannel(), battstate);
}

/**
 * @brief Stores the send loops to Cayenne.
 * 
 * This function stores the send loops to Cayenne.
 * 
 * @param cayenne Reference to the SmCayenne object.
 */
void Wmb::wmbadapter_addSendReadLoopsCounter(SmCayenne& cayenne)
{
	MyLog::log("WMB", "..add send loop counter to cayenne");

	cayenne.addSendReadLoops(m_smartmeter.getChannel(), m_sm_readLoops);
}

/**
 * @brief Stores the send fails counter to Cayenne.
 * 
 * This function stores the send fails counter to Cayenne.
 * 
 * @param cayenne Reference to the SmCayenne object.
 */
void Wmb::wmbadapter_addSendFailuresCounter(SmCayenne& cayenne)
{
	MyLog::log("WMB", "..add send failed counter to cayenne");

	cayenne.addSendFailures(m_smartmeter.getChannel(), m_send_fail);
}

/**
 * @brief Stores the adapter states to Cayenne.
 * 
 * This function stores the adapter states to Cayenne.
 * 
 * @param cayenne Reference to the SmCayenne object.
 */
void Wmb::wmbadaper_addStates(SmCayenne& cayenne)
{
	MyLog::log("WMB", "Add wmbadater states to cayenne");

	wmbadaper_battery(cayenne);
	wmbadapter_addSendReadLoopsCounter(cayenne);
	wmbadapter_addSendFailuresCounter(cayenne);
}
