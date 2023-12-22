/**
 * @file mylog.cpp
 * @brief Implementation of the MyLog class.
 * 
 * This file contains the implementation of the MyLog class, which provides logging functionality.
 * The MyLog class includes two logging methods: log and logHex.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include <cstdarg>
#include <string.h>
#include <stdio.h>

#include <mylog.h>

/**
 * @brief Logs a formatted message.
 * 
 * This function logs a formatted message using the specified tag and message format.
 * The message format supports variable arguments, similar to the printf function.
 * The log message is only generated if the MYLOG macro is defined.
 * 
 * @param tag The tag associated with the log message.
 * @param message The format string for the log message.
 * @param ... Variable arguments to be formatted and included in the log message.
 */
void MyLog::log(char const* tag, char const* message, ...)
{
    #ifdef MYLOG

        va_list args;
        va_start(args, message);

        size_t bufferSize = vsnprintf(nullptr, 0, message, args) * 2;

        char buffer[bufferSize];

        vsnprintf(buffer, bufferSize, message, args);
  
        MYLOG(tag, buffer);

    #endif
}

/**
 * @brief Logs a message with hexadecimal data.
 * 
 * This function logs a message with hexadecimal data using the specified tag, message, data, and size.
 * The log message is only generated if the MYLOG macro is defined.
 * 
 * @param tag The tag associated with the log message.
 * @param message The message to be logged before the hexadecimal data.
 * @param data The hexadecimal data to be logged.
 * @param size The size of the data array.
 */
void MyLog::logHex(char const* tag, char const* message, uint8_t const* data, size_t const size)
{
    #ifdef MYLOG

        size_t bufferSize = strnlen(message, 256) + (size * 4) + 2;

        char buffer[bufferSize];
        char* ptr = buffer;

        ptr += snprintf(buffer, sizeof(buffer), "%s ", message);

        for (size_t i = 0; i < size; i++)
        {
            ptr += snprintf(ptr, 5, "%02X ", data[i]);
        }

        MYLOG(tag, buffer);

    #endif
}
