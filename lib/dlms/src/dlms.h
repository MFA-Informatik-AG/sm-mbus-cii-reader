/**
 * @file dlms.h
 * @brief This file contains the declaration of the Dlms class.
 * 
 * The Dlms class provides functionality for handling DLMS (Device Language Message Specification) frames.
 * It includes methods for checking the LLC header, checking if a frame is a DLMS GBT frame,
 * handling received HDLC data, resetting the state, and checking if a GBT frame has been received.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "gbt.h"

class Dlms
{
    private:
        static size_t const MAX_DLMS_FRAME_SIZE = 1024;                                             // maximum size of a DLMS frame
        bool isLlcHeader(uint8_t const* data) const;                                                // checks if the LLC header is present
        bool isDlmsGbtFrame(uint8_t const* data) const;                                             // checks if the frame is a DLMS GBT frame
        Gbt& m_gbtFrame;                                                                            // reference to the Gbt object

    public:
        Dlms(Gbt& gbtFrame): m_gbtFrame(gbtFrame) {};                                               // constructor with reference to the Gbt object
        bool hdlcDataReceived(uint8_t const* data, size_t const size, GbtBlock const* addedBlock);  // processes HDLC data received intop the GBT frame
        void reset();                                                                               // resets the Dlms object, required for a new GBt frame                  
        bool gbtFrameReceived() const;                                                              // returns if a full GBT frame has been received                    
};
