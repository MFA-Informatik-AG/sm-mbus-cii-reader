/**
 * @file hdlc.cpp
 * @brief HDLC (High-Level Data Link Control) implementation.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include "Hdlc.h"
#include "mylog.h"

/**
 * @brief Constructs an instance of the Hdlc class.
 * 
 * @param hdlc_command_router The frame handler function pointer.
 */
Hdlc::Hdlc(frame_handler_type hdlc_command_router) : m_frame_handler(hdlc_command_router)
{
    m_receive_frame_position = 0;
    m_is_escape_character_received = false;
}

/**
 * @brief Calculates the FCS (Frame Check Sequence) for the given data.
 * 
 * @param fcs The initial FCS value.
 * @param cp Pointer to the data.
 * @param len Length of the data.
 * 
 * @return The calculated FCS value.
 */
uint16_t Hdlc::pppfcs16(uint16_t fcs, unsigned char *cp, int len)
{

    while (len--)
    {
        //
        // fcs >> 8:                                    This shifts the bits of fcs 8 places to the right. In effect, 
        //                                              this discards the rightmost 8 bits of fcs.
        //
        // fcs ^ *cp++:                                 This performs a bitwise XOR operation between fcs and the byte pointed to by cp. 
        //                                              The cp++ operation increments the pointer cp by one, so it points to the 
        //                                              next byte of data in each iteration.
        //
        // (fcs ^ *cp++) & 0xff:                        The & 0xff operation masks the result of the XOR operation, so only the 
        //                                              rightmost 8 bits are kept.
        //
        // fcstab[(fcs ^ *cp++) & 0xff]:                This uses the result of the masking operation as an index into the fcstab array. 
        //                                              fcstab is an array that contains precalculated results for the XOR operation 
        //                                              with each possible byte value.
        //
        //(fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]:    Finally, the result of the right shift operation is XORed with 
        //                                              the value retrieved from fcstab. This new value becomes the updated FCS.
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
    }

    return (fcs);
}

/**
 * @brief Swaps the bytes of a 16-bit unsigned integer.
 * 
 * @param val The input value.
 * @return The value with swapped bytes.
 */
uint16_t Hdlc::swap_uint16(uint16_t val) 
{
    return (val << 8) | (val >> 8 );
}

/**
 * @brief Receives a character and processes it as part of an HDLC frame.
 * 
 * @param received The received character.
 */
void Hdlc::charReceiver(uint8_t data)
{
    // start or end of hdlc frame received
    if (data == HDLC_FRAME_BOUNDARY_OCTET && m_is_escape_character_received == false)
    {
        // repeated 7E will be dropped
        if(m_receive_frame_position == 1) {

            return;
        }

        // full hdlc frame received with start/stop, flaggs and checksum
        _receive_frame_buffer[m_receive_frame_position] = data;

        // still within the hdlc header, no further processing of the received bytes
        // this might happen if an additional 7E is received as part of the hdlc frame
        // for data see is_escape_character_received of escaping the 7E
        if(m_receive_frame_position < 9)
        {
            m_receive_frame_position++;
    
            return;
        }

        // full dump of the hdlc frame buffer
        MyLog::logHex("HDLC", "receive_frame_buffer dump", _receive_frame_buffer, m_receive_frame_position);

        // copy the address, control, information of the hdlc buffer
        // the start/stop flag and the checksum will be dropped
        size_t content_frame_length = 0;

        for(size_t pos=0; pos < m_receive_frame_position - 3; pos++) 
        {
            _content_frame_buffer[pos]=_receive_frame_buffer[pos + 1];

            content_frame_length++;
        }

        MyLog::log("HDLC", "content_frame_buffer length %ld and position %ld", content_frame_length, m_receive_frame_position);
        
        // checksum is the last 2 bytes of the hdlc frame
        uint16_t checksumFrame = ((_receive_frame_buffer[m_receive_frame_position - 2] << 8) | (_receive_frame_buffer[m_receive_frame_position - 1] & 0xff));

        MyLog::log("HDLC", "content_frame_buffer calculate checksum");

        // calculate the checksum of the hdlc information block
        uint16_t checkSum = pppfcs16(PPPINITFCS16, _content_frame_buffer, content_frame_length);

        // invert the checksum
        uint16_t invertCheckSum = checkSum ^ 0xffff;                 

        // swap the bytes of the checksum
        uint16_t swapCheckSum = swap_uint16(invertCheckSum);

        MyLog::log("HDLC", "content_frame_buffer checksum calculated %ld expected %ld", swapCheckSum, checksumFrame);

        // compare the checksum of the hdlc frame with the calculated checksum
        if (checksumFrame == swapCheckSum)
        {
            MyLog::log("HDLC", "...call frame_handler with frame is valid");

            // call the frame handler with the hdlc information block and the frame is valid
            (*m_frame_handler)(_content_frame_buffer, content_frame_length, true);
        }
        else  
        {
            MyLog::log("HDLC", "...call frame_handler with frame is invalid");

            // call the frame handler with the hdlc information block and the frame is invalid (and yes I know this could be consolidated)
            (*m_frame_handler)(_content_frame_buffer, content_frame_length, false);
        }        

        MyLog::log("HDLC", "receive_frame_buffer position %u reset to 0", m_receive_frame_position);
        
        // reset the hdlc frame buffer
        m_receive_frame_position = 0;
    } 
    else 
    {
        // escape character received, the next byte will be inverted
        if (m_is_escape_character_received)
        {
          m_is_escape_character_received = false;
          
          data ^= INVERT_OCTET;
        }
        // escape character is only considered if within the information block (starting from the 9t byte
        else if (data == HDLC_CONTROL_ESCAPE_OCTET && m_receive_frame_position >= 9)
        {
            // escape character received within the information block, the next byte will be inverted
            m_is_escape_character_received = true;

            return;
        }

        // add the received byte to the hdlc frame buffer
        _receive_frame_buffer[m_receive_frame_position] = data;

        // increment the hdlc frame buffer position
        m_receive_frame_position++;
    }

    // hdlc frame buffer is full, reset the hdlc frame buffer
    if (m_receive_frame_position >= HDLC_MAX_FRAME_SIZE)
    {
        MyLog::log("HDLC", "receive_frame_buffer overflow. Reset the hdlc frame buffer position");
        
        // reset the hdlc frame buffer
        m_receive_frame_position = 0;
        m_is_escape_character_received = false;
    }

    return;
}

