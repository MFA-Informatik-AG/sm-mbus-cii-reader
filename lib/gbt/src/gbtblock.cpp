/**
 * @file gbtblock.cpp
 * @brief This file contains the implementation of the GbtBlock class.
 * 
 * The GbtBlock class represents a block of data in the GBT protocol. It provides methods to access
 * various properties of the block, such as block control, block number, block number acknowledge,
 * streaming status, window size, last block indicator, PDU content length, and copying PDU content
 * into a buffer.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include <string.h>

#include "gbtblock.h"

/**
 * @brief Constructs a GbtBlock object with the given data and size.
 * 
 * @param data A pointer to the data representing the GbtBlock frame.
 * @param size The size of the GbtBlock frame.
 */
GbtBlock::GbtBlock(uint8_t const* data, size_t const size)
{
    memcpy(m_gbtBlockFrame, data, size);
}

/**
 * @brief Gets the block control value of the GbtBlock.
 * 
 * @return The block control value.
 */
uint8_t GbtBlock::getBlockControl() const
{
    return m_gbtBlockFrame[1];
}

/**
 * @brief Gets the block number of the GbtBlock.
 * 
 * @return The block number (sequence number).
 */
uint16_t GbtBlock::getBlockNumber() const
{
    return m_gbtBlockFrame[3] | m_gbtBlockFrame[2] << 8;
}

/**
 * @brief Gets the block number acknowledge of the GbtBlock.
 * 
 * @return The block number acknowledge.
 */
uint16_t GbtBlock::getBlockNumberAcknowledge() const
{
    return m_gbtBlockFrame[5] | m_gbtBlockFrame[4] << 8;
}

/**
 * @brief Checks if the GbtBlock is in streaming mode.
 * 
 * @return True if the GbtBlock is in streaming mode, false otherwise.
 */
bool GbtBlock::isStreaming() const
{
    return (m_gbtBlockFrame[1] & 0x40) == 0x40;
}

/**
 * @brief Gets the window size of the GbtBlock.
 * 
 * @return The window size.
 */
uint8_t GbtBlock::windowSize() const
{
    return (m_gbtBlockFrame[1] & 0x3f);
}

/**
 * @brief Checks if the GbtBlock is the last block.
 * 
 * @return True if the GbtBlock is the last block, false otherwise.
 */
bool GbtBlock::isLastBlock() const
{
    return (m_gbtBlockFrame[1] & 0x80) != 0;
}

/**
 * @brief Gets the PDU content length of the GbtBlock.
 * 
 * @return The PDU content length.
 */
uint8_t GbtBlock::pduContentLength() const
{
    return m_gbtBlockFrame[6];
}

/**
 * @brief Copies the PDU content of the GbtBlock into the given buffer.
 * 
 * @param data A pointer to the buffer where the PDU content will be copied.
 * @param size The size of the buffer.
 * @return True if the PDU content was successfully copied, false otherwise.
 */
bool GbtBlock::pduContentCopyInto(uint8_t* data, uint8_t& size) const
{
    size = pduContentLength();

    memcpy(data, m_gbtBlockFrame + 7, size);

    return true;
}