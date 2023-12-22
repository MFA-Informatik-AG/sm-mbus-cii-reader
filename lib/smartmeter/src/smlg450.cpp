/**
 * @file smlg450.cpp
 * @brief Implementation of the SmLg450 class.
 * 
 * This file contains the implementation of the SmLg450 class, which is responsible for handling the communication
 * with the LG450 smart meter. It provides functions for opening and closing the serial port, reading data from the
 * serial port, parsing the received data, and copying the data to a Cayenne object.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include "gbtvaluebase.h"
#include "gbtuint8.h"
#include "gbtoctetstring.h"
#include "gbtuint16.h"
#include "gbtuint32.h"
#include "mylog.h"
#include "smlg450.h"

/**
 * @brief Closes the serial port.
 * 
 * This function closes the serial port used for communication with the LG450 smart meter.
 */
void SmLg450::closeSerialPort()
{
    MyLog::log("SMLG450", "closeSerialPort");

    Serial1.end();
}

/**
 * @brief Reads a byte from the serial port.
 * 
 * @return The byte read from the serial port.
 */
int SmLg450::read()
{
    return Serial1.read();
}

/**
 * @brief Checks if there is data available to read from the serial port.
 * 
 * @return The number of bytes available to read from the serial port.
 */
int SmLg450::available()
{
    return Serial1.available();
}

/**
 * @brief Opens the serial port.
 * 
 * This function opens the serial port used for communication with the LG450 smart meter.
 */
void SmLg450::openSerialPort()
{
    MyLog::log("SMLG450", "openSerialPort");

    // open and flush serial port 
	Serial1.begin(2400, SERIAL_8E1);

	Serial1.flush();
}

/**
 * @brief Copies the data from GbtData to a SmCayenne object.
 * 
 * @param gbtData The GbtData object containing the data to be copied.
 * @param cayenne The SmCayenne object to which the data will be copied.
 */
void SmLg450::copyData(GbtData const& gbtData, SmCayenne& cayenne)
{
    cayenne.reset();

    uint8_t i = 0;

    cayenne.addUnixTime(SMLG450CHANNEL, gbtData.getDateTime().asUnixTimeStamp());

    uint8_t valueItems = gbtData.getValueCount();

    while(i < valueItems)
    {
        GbtValueBase const *gbtValue = gbtData.getValue(i);

        if(gbtValue == nullptr)
        {
            MyLog::log("SMLG450", "Invalid GBT value returned for %d", i);

            continue;;
        }

        // GBT block
        if(gbtValue->getStructureIdent() == 4)
        {
            // describing structures are currently ignored 
            i += 4;
        }
        // LG block, order identifies the items based on the GBT block descriptions
        else
        {
            GbtValueBase const *gbtValue = gbtData.getValue(i);

            if(gbtValue == nullptr)
            {
                MyLog::log("SMLG450", "Invalid LG GBT value returned for %d", i);

                break;
            }

            uint8_t index = cayenne.addSmData(SMLG450CHANNEL, gbtValue);

            MyLog::log("SMLG450", "Add data for channel %d, index %d", SMLG450CHANNEL, index);
            
            i += 1;
        }
    }
}

/**
 * @brief Gets the date and time from the GbtData object.
 * 
 * @return The GbtDateTime object representing the date and time.
 */
GbtDateTime const& SmLg450::getDateTime()
{
    return m_gdbdateTime;
}

/**
 * @brief Parses the GbtData object and extracts relevant information.
 * 
 * @param gbtData The GbtData object to be parsed.
 * @return True if the parsing was successful, false otherwise.
 */
bool const SmLg450::parse(GbtData const& gbtData)
{
    MyLog::log("SMLG450", "Parse GBT data for SMLG450");

    uint8_t index = 0;

    // get the date and time from the GBT data
    auto const& gbtDateTime = gbtData.getDateTime();

    // copy the date and time into the member variable
    m_gdbdateTime.clone(gbtDateTime);

    uint8_t valueCount = gbtData.getValueCount();

    while(index < valueCount)
    {
        auto const* gbtValue = gbtData.getValue(index);

        if(gbtValue == nullptr)
        {
            MyLog::log("SMLG450", "Invalid GBT value returned for %d", index);

            index++;
        }
        // GBT block
        else if(gbtValue->getStructureIdent() == 4)
        {
            // describing structures are currently ignored
            index += 4;
        }
        // LG block, order identifies the items based on the GBT block descriptions
        else if(gbtValue->getValueType() == GbtValueBase::GbtValueType::GBTVALUETYPE_OCTETESTRING)
        {
            auto gbtIdentifier = static_cast<GbtOctetString const*>(gbtValue);

            // test for the device name identifier
            if(strcmp(gbtIdentifier->getStringValue(), "0.8.25.9.0.255") != 0)
            {
                continue;
            }
            
            // get the device name (in the next value)
            auto const* gbtValueDeviceNameData = gbtData.getValue(index + 1);

            // something went wrong
            if(gbtValueDeviceNameData == nullptr)
            {
                MyLog::log("SMLG450", "Null LG GBT value returned for index %d, expected pointer to device name, breaking loop", index + 1);

                break;
            }

            // check if the value is an octet string
            if(gbtValueDeviceNameData->getValueType() != GbtValueBase::GbtValueType::GBTVALUETYPE_OCTETESTRING)
            {
                MyLog::log("SMLG450", "Invalid LG GBT value returned for index %d, expected string for the device name, breaking loop", index + 1);

                break;
            }

            auto const* gbtValueDeviceName = static_cast<GbtOctetString const*>(gbtValueDeviceNameData);

            MyLog::log("SMLG450", "Found device name %s", gbtValueDeviceName->getStringValue());

            // copy the device name into the member variable
            strncpy(m_logicalDeviceName, gbtValueDeviceName->getStringValue(), gbtValueDeviceName->getStringLength() + 1);

            // point to the next value
            index += 14;
        }
        else 
        {
            // point to the next value
            index++;
        }
    }

    return true;
}


/**
 * @brief Get the channel number of the SmLg450 object.
 * 
 * @return The channel number.
 */
uint8_t SmLg450::getChannel()
{
    return SMLG450CHANNEL;
}


/**
 * @brief Get the logical device name.
 * 
 * This function returns the logical device name.
 * 
 * @return The logical device name.
 */
 char const* SmLg450::getLogicalDeviceName()
{
    return m_logicalDeviceName;
}
