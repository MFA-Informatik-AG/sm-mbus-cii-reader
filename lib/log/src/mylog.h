/**
 * @file mylog.h
 * @brief This file contains the declaration of the MyLog class and related macros for logging.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#if MY_BLE_DEBUG > 0
	#include <WisBlock-API-V2.h>
#endif

#if MY_UNIT_TEST > 0
	#ifndef MYLOG
		#define MYLOG(tag, ...)                     \
			do                                      \
			{                                       \
				if (tag)                            \
					printf("[%s] ", tag);           \
				printf(__VA_ARGS__);                \
				printf("\n");                       \
			} while (0)
	#endif
#endif

#ifdef NRF52_SERIES
    #ifndef MYLOG
		#if MY_DEBUG > 0 && MY_BLE_DEBUG == 0
			#define MYLOG(tag, ...)                     \
				do                                      \
				{                                       \
					if (tag)                            \
						printf("[%s] ", tag);           \
					printf(__VA_ARGS__);                \
					printf("\n");                       \
				} while (0)

		#elif MY_DEBUG > 0 && MY_BLE_DEBUG > 0
			#define MYLOG(tag, ...)                     \
				do                                      \
				{                                       \
					if (tag)                            \
						printf("[%s] ", tag);           \
					printf(__VA_ARGS__);                \
					printf("\n");                       \
					if (g_ble_uart_is_connected)        \
					{                                   \
						g_ble_uart.printf(__VA_ARGS__); \
						g_ble_uart.printf("\n");        \
					}                                   \
				} while (0)
		#else
			#define MYLOG(...)
		#endif
	#endif
#endif 

#ifdef ARDUINO_ARCH_RP2040
#if MY_DEBUG > 0
#define MYLOG(tag, ...)                  \
	do                                   \
	{                                    \
		if (tag)                         \
			Serial.printf("[%s] ", tag); \
		Serial.printf(__VA_ARGS__);      \
		Serial.printf("\n");             \
	} while (0)
#else
	#define MYLOG(...)
#endif
#endif

#if defined ESP32
    #ifndef MYLOG
	#include <Arduino.h>
		#if MY_DEBUG > 0 && MY_BLE_DEBUG == 0
			#define MYLOG(tag, ...)                                                 \
				if (tag)                                                            \
					Serial.printf("[%s] ", tag);                                    \
				Serial.printf(__VA_ARGS__);                                         \
				Serial.printf("\n");                                                
		#elif MY_DEBUG > 0 && MY_BLE_DEBUG > 0
			#define MYLOG(tag, ...)                                                 \
				if (tag)                                                            \
					Serial.printf("[%s] ", tag);                                    \
				Serial.printf(__VA_ARGS__);                                         \
				Serial.printf("\n");                                                \
				if (g_ble_uart_is_connected)                                        \
				{                                                                   \
					char buff[255];                                                 \
					int len = sprintf(buff, __VA_ARGS__);                           \
					uart_tx_characteristic->setValue((uint8_t *)buff, (size_t)len); \
					uart_tx_characteristic->notify(true);                           \
					delay(50);                                                      \
				}
		#else
			#define MYLOG(...)
		#endif
	#endif
#endif
class MyLog 
{
	public:
		static void log(char const* tag, char const* format, ...);
		static void logHex(char const* tag, char const* message, uint8_t const* data, size_t const size);
};