/**
 * @file smlg450.h
 * @brief The SmLg450 class represents a specific type of smart meter device, SmLg450.
 * 
 * This class inherits from the SmBase class and provides additional functionality specific to the SmLg450 device.
 * It includes methods for parsing data, retrieving the date and time, copying data to a Cayenne object,
 * opening and closing the serial port, and checking the availability of data.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include "smbase.h"

class SmLg450 : public SmBase
{
    private:
        static const uint8_t SMLG450CHANNEL = 10;                                                   // channel number for the SmLg450 device (identifies the device)
        static const uint8_t SM_MAX_DEVICENAME = 64;                                                // maximum length of the device name
        
        char m_logicalDeviceName[SM_MAX_DEVICENAME];                                                // logical device name of the smart meter
        GbtDateTime m_gdbdateTime;                                                                  // date and time information
        
    public:
        bool const parse(GbtData const& gbtData) override;
        GbtDateTime const& getDateTime() override;
        void copyData(GbtData const& gbtData, SmCayenne& cayenne) override;
        void openSerialPort() override;
        int available() override;
        int read() override;
        void closeSerialPort() override;
        uint8_t getChannel() override;
        char const* getLogicalDeviceName() override;
};


