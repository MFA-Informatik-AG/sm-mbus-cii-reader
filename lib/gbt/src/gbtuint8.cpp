/**
 * @file gbtuint8.cpp
 * @brief Implementation of the GbtUint8 class.
 * 
 * This file contains the implementation of the GbtUint8 class, which represents an 8-bit unsigned integer value in the GBT protocol.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include <string.h>
#include "gbtuint8.h"

/**
 * @brief Converts the GbtUint8 value to a string representation.
 * 
 * This function converts the GbtUint8 value to a string representation and stores it in the provided buffer.
 * 
 * @param buffer The buffer to store the string representation.
 * @param bufferSize The size of the buffer.
 */
void GbtUint8::asString(char* buffer, size_t const bufferSize) const
{
    snprintf(buffer, bufferSize, "GBT UINT8 Value %i, s=%i,a=%i", _uint8Value, m_structureIdent, m_arrayIdent);
}

/**
 * @brief Parses the GbtUint8 value from the given data.
 * 
 * This function parses the GbtUint8 value from the given data at the specified offset. It also updates the offset and sets the structure and array identifiers.
 * 
 * @param data The data to parse the value from.
 * @param offset The offset in the data where the value starts. This will be updated to the next offset after parsing.
 * @param structureIdent The structure identifier.
 * @param arrayIdent The array identifier.
 * @return True if the parsing was successful, false otherwise.
 */
bool GbtUint8::parse(const uint8_t* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent)
{
    _uint8Value = data[offset + 1];
    m_structureIdent = structureIdent;
    m_arrayIdent = arrayIdent;

    offset += 2;

    return true;
}

/**
 * @brief Gets the value of the GbtUint8.
 * 
 * This function returns the value of the GbtUint8.
 * 
 * @return The value of the GbtUint8.
 */
uint8_t GbtUint8::getValue() const
{
    return _uint8Value;
}

/**
 * @brief Gets the value type of the GbtUint8.
 * 
 * This function returns the value type of the GbtUint8, which is UINT8.
 * 
 * @return The value type of the GbtUint8.
 */
GbtValueBase::GbtValueType GbtUint8::getValueType() const
{
    return GbtValueBase::GbtValueType::GBTVALUETYPE_UINT8;
}