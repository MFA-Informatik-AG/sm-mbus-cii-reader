/**
 * @file gbtblock.h
 * @brief This file contains the declaration of the GbtBlock class.
 * 
 * The GbtBlock class represents a GBT block and provides methods to access
 * various properties of the block.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

class GbtBlock {

    private:
        uint8_t m_gbtBlockFrame[1024];                                               // buffer for the GBT block frame (assumption for max. 1024 bytes, subotimal)

    public:
        GbtBlock(uint8_t const* data, size_t const size);                           // constructor with pointer to the GBT block frame and its size
        uint8_t getBlockControl() const;                                            // returns the GBT block control byte
        uint16_t getBlockNumber() const;                                            // returns the GBT block number
        uint16_t getBlockNumberAcknowledge() const;                                 // returns the GBT block number acknowledge
        bool isStreaming() const;                                                   // checks if the GBT block is streaming
        uint8_t windowSize() const;                                                 // returns the GBT block window size
        bool isLastBlock() const;                                                   // returns if the GBT block is the last block
        uint8_t pduContentLength() const;                                           // returns the length of the PDU content
        bool pduContentCopyInto(uint8_t* value, uint8_t& size) const;               // copies the PDU out of the GBT block into the provided buffer
};