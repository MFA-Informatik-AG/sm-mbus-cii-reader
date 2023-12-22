/**
 * @file smat.h
 * @brief Header file for the smartmeter AT commands.
 * 
 * This file contains the declaration of AT commands for the SmartMeter module.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#pragma once

#include <WisBlock-API-V2.h>

#define AT_SM_READSENDCYCLE 0b1000000000000000          // AT command for initiate SmartMeter read cycle
#define AT_N_SM_READSENDCYCLE 0b0111111111111111        // reset AT command for initiate SmartMeter read cycle

atcmd_t *g_user_at_cmd_list;                            // AT command list required for wisblock
uint8_t g_user_at_cmd_num;                              // AT command number required for wisblock

void smcustom_at_init();                                // AT command initialization into wisblock