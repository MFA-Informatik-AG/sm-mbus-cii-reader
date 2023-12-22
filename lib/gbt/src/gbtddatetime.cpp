/**
 * @file gbtdatetime.cpp
 * @brief Represents a date and time in the GBT protocol.
 * 
 * This class provides methods to parse GBT date and time data, clone the object, 
 * convert it to a string representation, and obtain the Unix timestamp.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#include <time.h>
#include <stdio.h>

#include "gbtddatetime.h"

/**
 * @brief Get the year value of the GbtDateTime object.
 * 
 * @return The year value.
 */
uint16_t GbtDateTime::getYear() const
{
    return m_year;
}

/**
 * @brief Get the month value of the GbtDateTime object.
 *
 * This function returns the month value of the GbtDateTime object.
 *
 * @return The month value.
 */
uint8_t GbtDateTime::getMonth() const
{
    return m_month;
}


/**
 * @brief Get the day of the GbtDateTime object.
 * 
 * @return The day value.
 */
uint8_t GbtDateTime::getDay() const
{
    return m_day;
}

/**
 * @brief Get the hour value of the GbtDateTime object.
 *
 * @return The hour value.
 */
uint8_t GbtDateTime::getHour() const
{
    return m_hour;
}

/**
 * @brief Get the minute value of the GbtDateTime object.
 *
 * @return The minute value.
 */
uint8_t GbtDateTime::getMinute() const
{
    return m_minute;
}

/**
 * @brief Get the second value of the GbtDateTime object.
 * 
 * @return The second value.
 */
uint8_t GbtDateTime::getSecond() const
{
    return m_second;
}

/**
 * @brief Converts the GbtDateTime object to a Unix timestamp.
 * 
 * @return The Unix timestamp representing the GbtDateTime object.
 */
time_t GbtDateTime::asUnixTimeStamp() const 
{
    time_t rawtime;

    struct tm * timeinfo;

    time(&rawtime); 

    timeinfo = localtime(&rawtime); 

    timeinfo->tm_year   = this->getYear() - 1900;
    timeinfo->tm_mon    = this->getMonth() - 1;    
    timeinfo->tm_mday   = this->getDay();           
    timeinfo->tm_hour   = this->getHour();
    timeinfo->tm_min    = this->getMinute();
    timeinfo->tm_sec    = this->getSecond();

    time_t date = mktime(timeinfo);

    return date;
}

/**
 * @brief Clones the GbtDateTime object by copying the values from the source object.
 * 
 * @param source The GbtDateTime object to clone.
 */
void GbtDateTime::clone(GbtDateTime const& source)
{
    m_day = source.getDay();
    m_month = source.getMonth();
    m_year = source.getYear();
    m_hour = source.getHour();
    m_minute = source.getMinute();
    m_second = source.getSecond();
}

/**
 * @brief Parses the data buffer and updates the GbtDateTime object with the parsed values.
 * 
 * @param data The data buffer containing the date and time information.
 * @param offset The offset in the data buffer from where to start parsing.
 * @return true if the parsing is successful, false otherwise.
 */
bool GbtDateTime::parse(uint8_t const* data, size_t& offset, uint8_t const structureIdent, uint8_t const arrayIdent)
{
    m_structureIdent = structureIdent;
    m_arrayIdent = arrayIdent;

    m_year = data[offset + 1] << 8 | data[offset + 2];
    m_month = data[offset + 3];
    m_day = data[offset + 4];
    m_hour = data[offset + 6];
    m_minute = data[offset + 7];
    m_second = data[offset + 8];

    offset += 13;

    return true;
}

/**
 * Converts the GbtDateTime object to a string representation.
 * The resulting string is formatted as "dd.mm.yyyy hh:mm:ss".
 *
 * @param buffer The character array to store the resulting string.
 * @param bufferSize The size of the character array.
 */
void GbtDateTime::asString(char* buffer, size_t const bufferSize) const
{
    snprintf(buffer, bufferSize, "%02i.%02i.%04i %02i:%02i:%02i", m_day, m_month, m_year, m_hour, m_minute, m_second);
}

/**
 * @brief Returns the value type of GbtDateTime.
 * 
 * @return GbtValueBase::GbtValueType The value type of GbtDateTime.
 */
GbtValueBase::GbtValueType GbtDateTime::getValueType() const
{
    return GbtValueBase::GbtValueType::GBTVALUETYPE_NONE;
}