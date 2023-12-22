/**
 * @file gbtuint16.cpp
 * @brief Implementation of the GbtUint16 class.
 *
 * This file contains the implementation of the GbtUint16 class, which represents a 16-bit unsigned integer value in the GBT protocol.
 */

#include <string.h>

#include "gbtuint16.h"

/**
 * @brief Converts the GbtUint16 object to a string representation.
 *
 * This function converts the GbtUint16 object to a string representation and stores it in the provided buffer.
 *
 * @param buffer The buffer to store the string representation.
 * @param bufferSize The size of the buffer.
 */
void GbtUint16::asString(char* buffer, size_t const bufferSize) const
{
    snprintf(buffer, bufferSize, "GBT UINT16 Value %i, s=%i, a=%i", _uint16Value, m_structureIdent, m_arrayIdent);
}

/**
 * @brief Parses the GBT data and updates the GbtUint16 object.
 *
 * This function parses the GBT data starting from the specified offset and updates the GbtUint16 object with the parsed values.
 *
 * @param data The GBT data to parse.
 * @param offset The offset in the data to start parsing from.
 * @param structureIdent The structure identifier.
 * @param arrayIdent The array identifier.
 * @return True if the parsing is successful, false otherwise.
 */
bool GbtUint16::parse(const uint8_t* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent)
{
    _uint16Value = data[offset + 1] << 8 | data[offset + 2];
    m_structureIdent = structureIdent;
    m_arrayIdent = arrayIdent;

    offset += 3;

    return true;
}

/**
 * @brief Gets the value of the GbtUint16 object.
 *
 * This function returns the value of the GbtUint16 object.
 *
 * @return The value of the GbtUint16 object.
 */
uint16_t GbtUint16::getValue() const
{
    return _uint16Value;
}

/**
 * @brief Gets the value type of the GbtUint16 object.
 *
 * This function returns the value type of the GbtUint16 object.
 *
 * @return The value type of the GbtUint16 object.
 */
GbtValueBase::GbtValueType GbtUint16::getValueType() const
{
    return GbtValueBase::GbtValueType::GBTVALUETYPE_UINT16;
}