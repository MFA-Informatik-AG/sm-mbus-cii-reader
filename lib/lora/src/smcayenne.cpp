/**
 * @file smcayenne.cpp
 * @brief Implementation file for the SmCayenne class.
 * 
 * This file contains the implementation of the SmCayenne class, which provides functions for constructing Cayenne Low Power Payload (LPP) messages.
 * The SmCayenne class allows adding various types of sensor data to the LPP message buffer and encoding it according to the Cayenne LPP format.
 * It also provides functions for decoding received application settings from the LPP message buffer.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include "gbtuint8.h"
#include "gbtuint16.h"
#include "gbtuint32.h"
#include "gbtoctetstring.h"
#include "mylog.h"
#include "smcayenne.h"

/**
 * @brief Resets the SmCayenne object and the CayenneLPP buffer.
 * 
 * This function resets the SmCayenne object by setting the LPP index to 0 and calling the reset() function of the CayenneLPP base class.
 */
void SmCayenne::reset()
{
	m_lppIndex = 0;	

	CayenneLPP::reset();
}

/**
 * @brief Adds a battery voltage value to the LPP message buffer.
 * 
 * @param channel The channel number for the battery voltage value.
 * @param value The battery voltage value to be added.
 * @return The updated cursor position in the LPP message buffer.
 */
uint8_t SmCayenne::addBatteryVoltage(uint8_t channel, uint16_t value)
{
	// check buffer overflow
	if ((_cursor + SMLPP_UINT16_SIZE + SMLPP_HEADER_SIZE) > _maxsize)
	{
		_error = LPP_ERROR_OVERFLOW;
		
		return 0;
	}

	_buffer[_cursor++] = channel;
	_buffer[_cursor++] = SmCayenne::SMLPP_BATTERYVOLTAGE;
	_buffer[_cursor++] = SMLPP_UINT16_SIZE;

	_buffer[_cursor++] = (value >> 8) & 0xFF;
	_buffer[_cursor++] = (value) & 0xFF;

	return _cursor;
}

/**
 * @brief Adds the number of send failures to the LPP message buffer.
 * 
 * @param channel The channel number for the send failures value.
 * @param value The number of send failures to be added.
 * @return The updated cursor position in the LPP message buffer.
 */
uint8_t SmCayenne::addSendFailures(uint8_t channel, uint16_t value)
{
	// check buffer overflow
	if ((_cursor + SMLPP_UINT16_SIZE + SMLPP_HEADER_SIZE) > _maxsize)
	{
		_error = LPP_ERROR_OVERFLOW;
		
		return 0;
	}

	_buffer[_cursor++] = channel;
	_buffer[_cursor++] = SmCayenne::SMLPP_SENDFAILURES;
	_buffer[_cursor++] = SMLPP_UINT16_SIZE;

	_buffer[_cursor++] = (value >> 8) & 0xFF;
	_buffer[_cursor++] = (value) & 0xFF;

	return _cursor;
}

/**
 * @brief Adds the number of send/read loops to the LPP message buffer.
 * 
 * @param channel The channel number for the send/read loops value.
 * @param value The number of send/read loops to be added.
 * @return The updated cursor position in the LPP message buffer.
 */
uint8_t SmCayenne::addSendReadLoops(uint8_t channel, uint32_t value)
{
	// check buffer overflow
	if ((_cursor + SMLPP_UINT32_SIZE + SMLPP_HEADER_SIZE) > _maxsize)
	{
		_error = LPP_ERROR_OVERFLOW;
		
		return 0;
	}

	_buffer[_cursor++] = channel;
	_buffer[_cursor++] = SmCayenne::SMLPP_READLOOPS;
	_buffer[_cursor++] = SMLPP_UINT32_SIZE;

	_buffer[_cursor++] = (value >> 24) & 0xFF;
	_buffer[_cursor++] = (value >> 16) & 0xFF;
	_buffer[_cursor++] = (value >> 8) & 0xFF;
	_buffer[_cursor++] = (value) & 0xFF;

	return _cursor;
}

/**
 * @brief Adds sensor data to the LPP message buffer.
 * 
 * @param channel The channel number for the sensor data.
 * @param gbtValue A pointer to the GbtValueBase object containing the sensor data.
 * @return The updated cursor position in the LPP message buffer.
 */
uint8_t SmCayenne::addSmData(uint8_t channel, GbtValueBase const* gbtValue)
{
	uint8_t lppSize = getLppSize(gbtValue);

	if(lppSize == 0 || lppSize > 255)
	{
		return _cursor;
	}

	// check buffer overflow
	if ((_cursor + lppSize + SMLPP_HEADER_SIZE) > _maxsize)
	{
		_error = LPP_ERROR_OVERFLOW;
		
		return 0;
	}

	_buffer[_cursor++] = channel;
	_buffer[_cursor++] = m_lppIndex++;
	_buffer[_cursor++] = getLppValueType(gbtValue);

	appendLppValue(gbtValue);

	return _cursor;
}

/**
 * @brief Gets the LPP value type for a given GbtValueBase object.
 * 
 * @param gbtValue A pointer to the GbtValueBase object.
 * @return The LPP value type.
 */
uint8_t SmCayenne::getLppValueType(GbtValueBase const* gbtValue) const
{
	switch (gbtValue->getValueType())
	{
		case GbtValueBase::GbtValueType::GBTVALUETYPE_UINT8:
		{
			return SMLPP_UINT8_VALUETYPE;
		}

		case GbtValueBase::GbtValueType::GBTVALUETYPE_UINT16:
		{
			return SMLPP_UINT16_VALUETYPE;
		}

		case GbtValueBase::GbtValueType::GBTVALUETYPE_UINT32:
		{
			return SMLPP_UINT32_VALUETYPE;
		}

		case GbtValueBase::GbtValueType::GBTVALUETYPE_OCTETESTRING:
		{
			return SMLPP_OCTETESTRING_VALUETYPE;
		}

		default:
		{
			return SMLPP_UNKNOWN_VALUETYPE;
		}
	}
}

/**
 * @brief Appends the LPP value to the LPP message buffer.
 * 
 * @param gbtValue A pointer to the GbtValueBase object.
 * @return True if the LPP value was successfully appended, false otherwise.
 */
bool SmCayenne::appendLppValue(GbtValueBase const* gbtValue)
{
	switch (gbtValue->getValueType())
	{
		case GbtValueBase::GbtValueType::GBTVALUETYPE_UINT8:
		{
			auto gbt = static_cast<GbtUint8 const*>(gbtValue);

			_buffer[_cursor++] = gbt->getValue();
			return true;
		}

		case GbtValueBase::GbtValueType::GBTVALUETYPE_UINT16:
		{
			auto gbt = static_cast<GbtUint16 const*>(gbtValue);

			uint16_t value = gbt->getValue();
			_buffer[_cursor++] = (value >> 8) & 0xFF;
			_buffer[_cursor++] = (value) & 0xFF;
			
			return true;
		}

		case GbtValueBase::GbtValueType::GBTVALUETYPE_UINT32:
		{
			auto gbt = static_cast<GbtUint32 const*>(gbtValue);

			uint32_t value = gbt->getValue();    
			_buffer[_cursor++] = (value >> 24) & 0xFF;
			_buffer[_cursor++] = (value >> 16) & 0xFF;
			_buffer[_cursor++] = (value >> 8) & 0xFF;
			_buffer[_cursor++] = (value) & 0xFF;
			return true;
		}

		case GbtValueBase::GbtValueType::GBTVALUETYPE_OCTETESTRING:
		{
			auto gbt = static_cast<GbtOctetString const*>(gbtValue);

			char const* value = gbt->getStringValue();
			size_t valueLength = gbt->getStringLength();

			for (size_t i = 0; i < valueLength; i++)
			{
				_buffer[_cursor++] = value[i];
			}

			_buffer[_cursor++] = '\0';

			return true;
		}

		default:
		{
			return false;
		}
	}
}

/**
 * @brief Gets the LPP size for a given GbtValueBase object.
 * 
 * @param gbtValue A pointer to the GbtValueBase object.
 * @return The LPP size.
 */
uint8_t SmCayenne::getLppSize(GbtValueBase const* gbtValue) const
{
	switch (gbtValue->getValueType())
	{
		case GbtValueBase::GbtValueType::GBTVALUETYPE_UINT8:
		{
			return SMLPP_UINT8_SIZE;
		}

		case GbtValueBase::GbtValueType::GBTVALUETYPE_UINT16:
		{
			return SMLPP_UINT16_SIZE;
		}

		case GbtValueBase::GbtValueType::GBTVALUETYPE_UINT32:
		{
			return SMLPP_UINT32_SIZE;
		}

		case GbtValueBase::GbtValueType::GBTVALUETYPE_OCTETESTRING:
		{
			auto gbt = static_cast<GbtOctetString const*>(gbtValue);

			return gbt->getStringLength();
		}

		default:
		{
			MyLog::log("SMCAYENNE", "Unknown GBT value type %d", gbtValue->getValueType());

			return 0;
		}
	}
}

/**
 * @brief Decodes the received application settings from the LPP message buffer.
 * 
 * @param data A pointer to the received data buffer.
 * @param size The size of the received data buffer.
 * @param config The AppConfig object to store the decoded application settings.
 */
void SmCayenne::smDecodeReceivedAppSettings(uint8_t const* data, size_t size, AppConfig& config)
{
	size_t index = 0;

	while (index < size)
	{
		uint8_t channel = data[index++];
		uint8_t type = data[index++];

		if (channel == SMDECODE_CHANNEL_MEASUREINTERVAL)
		{
			if (type == SMDECODE_UINT32)
			{
				config.measureInterval = getUint32FromByteArray(data, index);
			}
		}

		if (channel == SMDECODE_CHANNEL_CYCLETIMEOUT)
		{
			if (type == SMDECODE_UINT32)
			{
				config.smCycleTimeout = getUint32FromByteArray(data, index);
			}
		}

		if (channel == SMDECODE_CHANNEL_SENDDATATYPE)
		{
			if (type == SMDECODE_UINT8)
			{
				config.sendDataType = getUint8FromByteArray(data, index);
			}
		}

		if (channel == SMDECODE_CHANNEL_DECRYPTDATA)
		{
			if (type == SMDECODE_BOOL)
			{
				config.decryptData = getBoolFromByteArray(data, index);
			}
		}

		if (channel == SMDECODE_CHANNEL_AUTHENTICATIONKEY)
		{
			if (type == SMDECODE_BYTEARRAY)
			{
				for (uint8_t i = 0; i < 16; i++)
				{
					config.authenticationKey[i] = data[index++];
				}
			}
		}

		if (channel == SMDECODE_CHANNEL_AESKEY)
		{
			if (type == SMDECODE_BYTEARRAY)
			{
				for (uint8_t i = 0; i < 16; i++)
				{
					config.aes_key[i] = data[index++];
				}
			}
		}

		if (channel == SMDECODE_CHANNEL_AESIV)
		{
			if (type == SMDECODE_BYTEARRAY)
			{
				for (uint8_t i = 0; i < 16; i++)
				{
					config.aes_iv[i] = data[index++];
				}
			}
		}
	}
}

/**
 * @brief Gets a uint8 value from a byte array and updates the offset.
 * 
 * @param data A pointer to the byte array.
 * @param offset A reference to the offset variable.
 * @return The uint8 value.
 */
uint8_t SmCayenne::getUint8FromByteArray(uint8_t const* data, size_t& offset)
{
	return data[offset++];
}

/**
 * @brief Gets a bool value from a byte array and updates the offset.
 * 
 * @param data A pointer to the byte array.
 * @param offset A reference to the offset variable.
 * @return The bool value.
 */
bool SmCayenne::getBoolFromByteArray(uint8_t const* data, size_t& offset)
{
	return data[offset++] == 1;
}

/**
 * @brief Gets a uint32 value from a byte array and updates the offset.
 * 
 * @param data A pointer to the byte array.
 * @param offset A reference to the offset variable.
 * @return The uint32 value.
 */
uint32_t SmCayenne::getUint32FromByteArray(uint8_t const* data, size_t& offset)
{
	uint32_t value = 0;

	value = data[offset++] << 24;
	value |= data[offset++] << 16;
	value |= data[offset++] << 8;
	value |= data[offset++];

	return value;
}

