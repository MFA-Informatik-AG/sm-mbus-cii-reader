/**
 * @file main.h
 * @brief This file contains the declaration of the main class.
 *  
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#define SM_RF52USB_SERIAL_DELAY     5000            // delay for nf52 serial port in ms
#define SM_BLE_DEVICENAME           "MFASM"         // device name for BLE (wisblock)

#define CAYENNEPAYLOAD			    250			    // max 250 bytes of data for cayenne

#define SW_VERSION_1 1 						        // major version 
#define SW_VERSION_2 0 						        // minor version 
#define SW_VERSION_3 4 						        // patch version 

void gbt_frame_handler(uint8_t const* data, size_t const size);                                 // handle GBT data  
void hdlc_frame_handler(uint8_t const* data, size_t const size, bool const valid);              // handle HDLC data