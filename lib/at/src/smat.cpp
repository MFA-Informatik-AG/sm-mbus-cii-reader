/**
 * @file smat.cpp
 * @brief Implementation of AT commands for SmartMeter module.
 * 
 * This file contains the implementation of AT commands for the SmartMeter module.
 * It includes functions to retrieve and set the measurement interval, run a read cycle,
 * and reset the stored configuration to default values.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#include <WisBlock-API-V2.h>

#ifdef NRF52_SERIES
#include "appsettings-nrf52.h"
#endif 

#ifdef ARDUINO_ARCH_ESP32
//todo: add esp32 mcu
#endif

#include "mylog.h"
#include "smat.h"

/**
 * @brief Retrieves the measurement interval.
 *
 * This function returns the measurement interval used by the AT module.
 *
 * @return The measurement interval.
 */
static int at_query_measurementinterval()
{
	AppConfig appConfig;

	AppSettings::loadConfiguration(appConfig);

	snprintf(g_at_query_buf, ATQUERY_SIZE, "SmartMeter Measurement interval: %ld ms", appConfig.measureInterval);

	return 0;
}

/**
 * @brief Executes the measurement interval command.
 *
 * This function sets the measurement interval used by the AT module to the specified value.
 *
 * @param str The command string containing the new measurement interval value.
 * @return The result of the execution.
 */
static int at_exec_measurementinterval(char *str)
{
	uint32_t new_val = strtol(str, nullptr, 0);

	MyLog::log("APP", "Set SmartMeter measurement interval (wakeup timer) to %ld ms", new_val);

	AppConfig appConfig;

	AppSettings::loadConfiguration(appConfig);

	appConfig.measureInterval = new_val;

	AppSettings::saveConfiguration(appConfig);

	return 0;
}

/**
 * @brief Executes the read cycle command.
 *
 * This function executes a read cycle for the SmartMeter module.
 *
 * @return The result of the execution.
 */
static int at_cmd_runcycle()
{
	api_wake_loop(AT_SM_READSENDCYCLE);

	return 0;
}

/**
 * @brief Executes the reset flash command.
 *
 * This function resets the stored configuration of the SmartMeter module to default values.
 *
 * @return The result of the execution.
 */
static int at_cmd_resetflash()
{
	AppConfig appConfig;

	AppSettings::resetConfiguration(appConfig);

	return 0;
}

/**
 * @brief Example user-defined AT command list.
 * 
 * This array stores user-defined AT commands.
 * Each element of the array represents a single AT command.
 * The format of each element is defined by the atcmd_t structure.
 */
atcmd_t g_user_at_cmd_list_example[] = {
	/*|    CMD    |     AT+CMD?      |    AT+CMD=?    |  AT+CMD=value |  AT+CMD  |  Permission  |*/
	// GNSS commands
	{"+SMMINT", "Get/Set SmartMeter measurement interval (wakeup timer) in ms", at_query_measurementinterval, at_exec_measurementinterval, NULL, "RW"},
	{"+SMREAD", "Run a SmartMeter read cycle with data transmision", NULL, NULL, at_cmd_runcycle, "R"},
	{"+SMRESETCONFIG", "Reset the stored configuration to the default values", NULL, NULL, at_cmd_resetflash, "R"}
};


/**
 * @brief Initializes the custom AT command list.
 * 
 * This function assigns the example AT command list to the global variable `g_user_at_cmd_list`
 * and sets the number of AT commands in the list to `g_user_at_cmd_num`.
 */
void smcustom_at_init() 
{
	g_user_at_cmd_list = g_user_at_cmd_list_example;
	g_user_at_cmd_num = sizeof(g_user_at_cmd_list_example) / sizeof(atcmd_t);
}

