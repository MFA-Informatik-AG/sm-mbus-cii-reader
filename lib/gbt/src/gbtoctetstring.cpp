/**
 * @file gbtoctetstring.cpp
 * @brief Represents an octet string value in the GBT protocol.
 * 
 * This class inherits from GbtValueBase and provides functionality to parse, convert to string,
 * and compare octet string values.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include <string.h>

#include "gbtoctetstring.h"

/**
 * @brief Converts the GBT octet string to a string representation.
 * 
 * This function converts the GBT octet string to a string representation
 * and stores it in the provided buffer.
 * 
 * @param buffer The buffer to store the string representation.
 * @param bufferSize The size of the buffer.
 */
void GbtOctetString::asString(char* buffer, size_t const bufferSize) const
{
    snprintf(buffer, bufferSize, "GBT string Value %s, s=%i, a=%i", _stringValue, m_structureIdent, m_arrayIdent);
}

/**
 * @brief Parses the GBT octet string from the given data.
 * 
 * This function parses the GBT octet string from the given data and updates
 * the internal state of the object.
 * 
 * @param data The data containing the GBT octet string.
 * @param offset The offset in the data where the GBT octet string starts.
 * @param structureIdent The structure identifier.
 * @param arrayIdent The array identifier.
 * @return True if the parsing is successful, false otherwise.
 */
bool GbtOctetString::parse(uint8_t const* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent)
{
    m_structureIdent = structureIdent;
    m_arrayIdent = arrayIdent;

    // get the lenght of the string (single numbers as bytes)
    size_t length = data[offset + 1];

    // check for potential buffer overflow
    if(length > MAX_GBTSTRINGSIZE)
    {
        return false;
    }

    // get the value of the last byte    
    uint8_t lastValue = data[offset + 1 + length];

    // check if it is an string of octetes which can be formatet in an 1.2.3.4.5 way
    if(lastValue == 0xff)
    {
        // initialize the string
        strcpy(_stringValue, "");

        for(size_t i=0; i < length; i++)
        {
            char buf[10];

            // convert the byte to a string
            sprintf(buf, "%d", data[offset + 2 + i]);

            // append the string to the GBT octet string
            strncat(_stringValue, buf, 10);

            // add a dot if there are more bytes to come
            if(i + 1 < length)
            {
                strcat(_stringValue, ".");
            }
        }
    }
    else 
    {
        // copy the bytes as characters into the string buffer
        for(size_t i=0; i < length; i++)
        {
            _stringValue[i] = data[offset + 2 + i];
        }

        // append 0 byte to the string for termination
        _stringValue[length] = 0;
    }

    // move the pointer to the next value
    offset = offset + 2 + length;

    return true;
}


/**
 * Retrieves the string value of the GbtOctetString.
 *
 * @return The string value of the GbtOctetString.
 */
char const* GbtOctetString::getStringValue() const 
{
    return _stringValue;
}

/**
 * @brief Get the length of the octet string.
 * 
 * This function returns the length of the octet string, excluding the null terminator.
 * 
 * @return The length of the octet string.
 */
size_t GbtOctetString::getStringLength() const
{
    return strnlen(_stringValue, MAX_GBTSTRINGSIZE);
}

/**
 * @brief Compares the GBT octet string with the given string.
 * 
 * This function compares the GBT octet string with the given string.
 * 
 * @param compareWith The string to compare with.
 * @return True if the strings are equal, false otherwise.
 */
GbtValueBase::GbtValueType GbtOctetString::getValueType() const
{
    return GbtValueBase::GbtValueType::GBTVALUETYPE_OCTETESTRING;
}