/**
 * @file smbase.h
 * @brief Defines the SmBase class, which is a base class for smart meter communication.
 * 
 * This file contains the declaration of the SmBase class, which provides an interface for parsing
 * smart meter data, retrieving date and time information, copying data to a Cayenne object,
 * opening and closing a serial port, and reading data from the serial port.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gbtdata.h"
#include "smcayenne.h"

class SmBase 
{
    public:
        virtual ~SmBase() = default;
        virtual bool const parse(GbtData const& gbtData) = 0;                                       // parse the smart meter data from the GbtData object
        virtual GbtDateTime const& getDateTime() = 0;                                               // get the date and time information
        virtual void copyData(GbtData const& gbtData, SmCayenne& cayenne) = 0;                      // copy the smart meter data to the Cayenne object
        virtual void openSerialPort() = 0;                                                          // open the serial port for reading on the smart meter
        virtual int available() = 0;                                                                // check if data is available on the serial port
        virtual int read() = 0;                                                                     // read a byte from the serial port
        virtual void closeSerialPort() = 0;                                                         // close the serial port
        virtual uint8_t getChannel() = 0;                                                           // get the channel number of the smart meter (identifies the device)
        virtual char const* getLogicalDeviceName() = 0;                                             // get the logical device name of the smart meter
};