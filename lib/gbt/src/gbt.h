/**
 * @file gbt.h
 * @brief This file contains the declaration of the Gbt class.
 * 
 * The Gbt class provides functionality for handling GBT (Generic Binary Transfer) frames.
 * It allows joining GBT blocks, adding PDUs (Protocol Data Units), and checking if a GBT frame has been received.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "gbtblock.h"


/**
 * @brief Type definition for a GBT handler function.
 *
 * This typedef defines the function signature for a GBT handler, which takes a pointer to
 * constant data and its size as parameters.
 *
 * @param data Pointer to the constant data.
 * @param size Size of the data.
 */
typedef void (*gbt_handler_type)(uint8_t const* data, size_t const size);

class Gbt {

    private:
        static uint8_t const MAX_GBTBLOCKS = 50;                                            // maximum number of single GBT blocks

        gbt_handler_type m_gbtFrameHandler;                                                 // pointer to the GBT frame handler function

        uint8_t m_gbtBlockCounter = 0;                                                      // counter for the number of received GBT blocks       
        bool m_gbtReceived = false;                                                         // flag indicating if a full GBT frame (several chunks) has been received

        GbtBlock* m_gbtBlocks[MAX_GBTBLOCKS];                                               // array of pointers to GBT blocks (m_gbtBlockCounter is the number of valid pointers in the array)
        GbtBlock const* addGbtBlock(uint8_t const* data, size_t const size);                // adds a GBT block to the array of GBT blocks
        bool joinGbtBlocks(uint8_t* data, size_t const size);                               // joins the GBT blocks chunks into to a single GBT frame
        
    public:
        ~Gbt();
        Gbt(gbt_handler_type gbtHandler);
        void reset();                                                                       // resets the GBT object and m_gbtFrameHandler counter
        bool addPdu(uint8_t const* data, size_t const size, GbtBlock const* addBlock);      // adds a PDU to the GBT frame
        bool gbtFrameReceived() const;                                                      // checks if a GBT frame has been received
};