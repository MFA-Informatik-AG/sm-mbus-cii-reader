/**
 * @file dlms.cpp
 * @brief Implementation file for the Dlms class.
 * 
 * This file contains the implementation of the Dlms class, which provides
 * functionality for handling DLMS (Device Language Message Specification) frames.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include "dlms.h"
#include "mylog.h"

/**
 * @brief Resets the Dlms object.
 * 
 * This function resets the internal state of the Dlms object.
 */
void Dlms::reset()
{
    m_gbtFrame.reset();
}

/**
 * @brief Checks if a GBT frame has been received.
 * @return true if a GBT frame has been received, false otherwise.
 */
bool Dlms::gbtFrameReceived() const
{
    return m_gbtFrame.gbtFrameReceived();
}

/**
 * @brief Processes HDLC data received.
 * 
 * This function processes the HDLC data received and adds it to the GBT frame.
 * 
 * @param data Pointer to the HDLC data.
 * @param size Size of the HDLC data.
 * @param addedBlock Pointer to the added GbtBlock (can be used for further imediate processing or analysis).
 * @return true if the data was successfully added to the GBT frame, false otherwise.
 */
bool Dlms::hdlcDataReceived(uint8_t const* data, size_t const size, GbtBlock const* addedBlock)
{
    if(data == nullptr || size == 0)
    {
        return false;
    }

    size_t pos = 0;

    if(isLlcHeader(data)) 
    {
        MyLog::log("HDLC", "LLC header detected");

        // forwards the LLC header to the GBT frame
        pos = 3;
    }

    // only add the data if it is a GBT frame
    if(isDlmsGbtFrame(data + pos))
    {
        // add the data to the GBT frame
        return m_gbtFrame.addPdu(data + pos, size - pos, addedBlock);
    }

    MyLog::logHex("HDLC", "Not a GBT frame", data, size);

    return false;
}

/**
 * @brief Checks if the given data is a DLMS GBT frame.
 * 
 * @param data Pointer to the data.
 * @return true if the data is a DLMS GBT frame, false otherwise.
 */
bool Dlms::isDlmsGbtFrame(uint8_t const* data) const
{
    if(data == nullptr)
    {
        return false;
    }

    // check if the data is a GBT frame (starts with GBT frame identifier)
    return data[0] == 0xe0;
}

/**
 * @brief Checks if the given data is an LLC header.
 * 
 * @param data Pointer to the data.
 * @return true if the data is an LLC header, false otherwise.
 */
bool Dlms::isLlcHeader(uint8_t const* data) const
{
    if(data == nullptr)
    {
        return false;
    }

    // check if the data is an LLC header (starts with LLC header identifier 0xe6 and 0xe7)
    return data[0]==0xe6 && data[1]==0xe7;
}
    

