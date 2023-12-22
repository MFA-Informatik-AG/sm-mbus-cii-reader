/**
 * @file gbtuint32.cpp
 * @brief Implementation of the GbtUint32 class.
 * 
 * This file contains the implementation of the GbtUint32 class, which represents a 32-bit unsigned integer
 * value in the GBT protocol. It provides methods for cloning, converting to string, parsing from data, and
 * retrieving the value.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include <string.h>
#include "gbtuint32.h"

/**
 * @brief Clones the GbtUint32 object.
 * 
 * This function clones the GbtUint32 object by copying the value from the source object.
 * 
 * @param source The GbtUint32 object to clone.
 */
void GbtUint32::clone(GbtUint32 const& source) 
{
    _uint32Value = source.getValue();
}

/**
 * @brief Converts the GbtUint32 object to a string.
 * 
 * This function converts the GbtUint32 object to a string representation and stores it in the provided buffer.
 * 
 * @param buffer The buffer to store the string representation.
 * @param bufferSize The size of the buffer.
 */
void GbtUint32::asString(char* buffer, size_t const bufferSize) const
{
    snprintf(buffer, bufferSize, "GBT UINT32 Value %lu, s=%d,a=%u", _uint32Value, m_structureIdent, m_arrayIdent);
}

/**
 * @brief Parses the GbtUint32 object from data.
 * 
 * This function parses the GbtUint32 object from the provided data buffer at the specified offset. It also sets
 * the structure identifier and array identifier.
 * 
 * @param data The data buffer to parse from.
 * @param offset The offset in the data buffer to start parsing from.
 * @param structureIdent The structure identifier.
 * @param arrayIdent The array identifier.
 * @return True if the parsing is successful, false otherwise.
 */
bool GbtUint32::parse(const uint8_t* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent)
{
    _uint32Value = data[offset + 1] << 24 | data[offset + 2] << 16 | data[offset + 3] << 8 | data[offset + 4];
    m_structureIdent = structureIdent;
    m_arrayIdent = arrayIdent;

    offset += 5;

    return true;
}

/**
 * @brief Retrieves the value of the GbtUint32 object.
 * 
 * This function retrieves the value of the GbtUint32 object.
 * 
 * @return The value of the GbtUint32 object.
 */
uint32_t GbtUint32::getValue() const
{
    return _uint32Value;
}

/**
 * @brief Returns the value type of GbtUint32.
 * 
 * @return The value type of GbtUint32, which is GbtValueType::GBTVALUETYPE_UINT32.
 */

GbtValueBase::GbtValueType GbtUint32::getValueType() const
{
    return GbtValueBase::GbtValueType::GBTVALUETYPE_UINT32;
}