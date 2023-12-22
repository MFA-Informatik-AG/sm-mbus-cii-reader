/**
 * @file gbt.cpp
 * @brief Implementation of the Gbt class.
 * 
 * This file contains the implementation of the Gbt class, which represents a GBT (Generic Binary Transfer) object.
 * The Gbt class is responsible for managing GbtBlocks, which are used to store and process data received in GBT frames.
 * It provides functions for adding GbtBlocks, checking if a GBT frame has been received, and joining GbtBlocks into a single PDU (Protocol Data Unit) buffer.
 * The Gbt class also includes a handler function for processing GBT frames.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#include "mylog.h"
#include "gbt.h"


/**
 * @brief Constructor for the Gbt class.
 *
 * @param gbtHandler The handler function for GBT frames.
 */
Gbt::Gbt(gbt_handler_type gbtHandler) : m_gbtFrameHandler(gbtHandler)
{
    for(uint8_t i=0; i < MAX_GBTBLOCKS; i++)
    {
        m_gbtBlocks[i] = nullptr;
    }
}

/**
 * @brief Destructor for the Gbt class.
 */
Gbt::~Gbt()
{   
    reset();
}

/**
 * @brief Resets the Gbt object by deleting all allocated GbtBlocks and resetting the counters.
 */
void Gbt::reset()
{
    for(uint8_t i=0; i < MAX_GBTBLOCKS; i++)
    {
        if(m_gbtBlocks[i] != nullptr)
        {
            delete m_gbtBlocks[i];

            m_gbtBlocks[i] = nullptr;
        }
    }

    m_gbtBlockCounter = 0;
    m_gbtReceived = false;
}

/**
 * @brief Checks if a GBT frame has been received.
 * 
 * @return true if a GBT frame has been received, false otherwise.
 */
bool Gbt::gbtFrameReceived() const
{
    return m_gbtReceived;
}

/**
 * @brief Adds a new GbtBlock to the Gbt object.
 *
 * This function creates a new GbtBlock object using the provided data and size,
 * and adds it to the Gbt object's internal array of GbtBlocks. If there is already
 * a GbtBlock at the current position in the array, it is deleted before the new
 * GbtBlock is added.
 *
 * @param data A pointer to the data for the GbtBlock.
 * @param size The size of the data for the GbtBlock.
 * @return A pointer to the newly created GbtBlock.
 */
GbtBlock const* Gbt::addGbtBlock(uint8_t const* data, size_t const size) 
{
    if(m_gbtBlocks[m_gbtBlockCounter] != nullptr)
    {
        delete m_gbtBlocks[m_gbtBlockCounter];

        m_gbtBlocks[m_gbtBlockCounter] = nullptr;
    }

    return m_gbtBlocks[m_gbtBlockCounter++] = new GbtBlock(data, size);
}

/**
 * @brief Adds a PDU (Protocol Data Unit) to the Gbt object.
 * 
 * This function adds a PDU to the Gbt object and performs necessary checks and actions based on the added block.
 * If the block number of the added block does not match the expected block number, the function resets the Gbt object.
 * If the block number is not 1, it skips blocks until a new first block is received.
 * If the added block is the last block of a sequence, it joins all the received blocks and calls the GBT block handler with the joined buffer.
 * 
 * @param data Pointer to the data of the PDU.
 * @param size Size of the PDU data.
 * @param addBlock Pointer to the GbtBlock object representing the added block.
 * @return True if the PDU was successfully added, false otherwise.
 */
bool Gbt::addPdu(uint8_t const* data, size_t const size, GbtBlock const* addBlock)
{
    addBlock = addGbtBlock(data, size); 

    if(addBlock->getBlockNumber() != m_gbtBlockCounter)
    {
        reset();

        if(addBlock->getBlockNumber() != 1)
        {
            MyLog::log("GBT", "Block number missmatch, skip blocks till a new first block is received");

            return false;
        }

        MyLog::log("GBT", "Restart sequence recording with the new first block");

        addBlock = addGbtBlock(data, size); 
    }

    if(addBlock->isLastBlock())
    {
        MyLog::log("GBT", "Last sequence block received");

        size_t bufferSize = 0;

        for(uint8_t i=0; i<m_gbtBlockCounter; i++)
        {
            bufferSize += m_gbtBlocks[i]->pduContentLength();
        }

        uint8_t receivedPduBuffer[bufferSize];

        if(joinGbtBlocks(receivedPduBuffer, bufferSize))
        {
            MyLog::log("GBT", "...call GBT block handler with buffer of %d bytes", bufferSize);

            MyLog::logHex("GBT", "...joined GBT block: ", receivedPduBuffer, bufferSize);

            m_gbtFrameHandler(receivedPduBuffer, bufferSize);

            m_gbtReceived = true;

            MyLog::log("GBT", "...GBT block completed");
        }

        MyLog::log("GBT", "Last block completed");
    }

    return true;
}

/**
 * Joins the GBT blocks into a single PDU buffer.
 * 
 * @param buffer The buffer to store the joined GBT blocks.
 * @param bufferSize The size of the buffer.
 * @return True if the GBT blocks were successfully joined, false otherwise.
 */
bool Gbt::joinGbtBlocks(uint8_t* buffer, size_t const bufferSize) 
{
    MyLog::log("GBT", "Join GBT blocks %d with a max buffer size of %d", m_gbtBlockCounter, bufferSize);

    size_t pduBufferPos = 0;
    
    uint8_t pduValue[bufferSize];
    uint8_t pduValueSize=0;

    for(uint8_t i=0; i<m_gbtBlockCounter; i++)
    {
        if(!m_gbtBlocks[i]->pduContentCopyInto(pduValue, pduValueSize))
        {
            break;
        }

        if(pduBufferPos + pduValueSize > bufferSize)
        {
            MyLog::log("GBT", "Buffer overflow, joined GBT blocks are larger than the buffer size %d", pduBufferPos + pduValueSize);

            return false;
        }

        for (uint8_t x = 0; x < pduValueSize; x++)
        {
            buffer[pduBufferPos++] = pduValue[x];
        }
    }

    if(pduBufferPos > bufferSize)
    {
        MyLog::log("GBT", "Buffer overflow, joined GBT blocks are larger than the buffer size %d", pduBufferPos);

        return false;
    }

    MyLog::logHex("GBT", "Joined GBT blocks to PDU: ", buffer, bufferSize);

    return  true;
}
