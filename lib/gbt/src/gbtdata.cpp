/**
 * @file gbtdata.cpp
 * @brief Implementation of the GbtData class.
 * 
 * This file contains the implementation of the GbtData class, which represents a collection of GbtValueBase objects.
 * It provides methods for accessing and parsing the data.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */

#include "gbtdata.h"
#include "gbtuint16.h"
#include "gbtuint8.h"
#include "gbtoctetstring.h"
#include "mylog.h"

/**
 * @brief Default constructor for the GbtData class.
 * 
 * Initializes the GbtData object with default values.
 */
GbtData::GbtData()
{
    m_gbtValueCount = 0;

    // better save than sorry
    for (size_t i = 0; i < MAX_GBTVALUES; i++)
    {
        m_gbtValues[i] = nullptr;
    }
}

/**
 * @brief Destructor for the GbtData class.
 * 
 * Cleans up the memory allocated for GbtValueBase objects.
 */
GbtData::~GbtData()
{
    for(size_t i=0; i < MAX_GBTVALUES; i++)
    {
        if(m_gbtValues[i] != nullptr)
        {
            delete m_gbtValues[i];
        }
    }
}

/**
 * @brief Get the date and time associated with the GbtData object.
 * 
 * @return A constant reference to the GbtDateTime object representing the date and time.
 */
GbtDateTime const& GbtData::getDateTime() const
{
    return m_dateAndTime; 
}

/**
 * @brief Get the long invoked priority ID associated with the GbtData object.
 * 
 * @return A constant reference to the GbtUint32 object representing the long invoked priority ID.
 */
GbtUint32 const& GbtData::getLongInvokedPriorityId() const
{
    return m_longInvokedPriorityId;
}

/**
 * @brief Get the number of GbtValueBase objects stored in the GbtData object.
 * 
 * @return The number of GbtValueBase objects.
 */
uint8_t GbtData::getValueCount() const
{
    return m_gbtValueCount;
}

/**
 * @brief Get a specific GbtValueBase object from the GbtData object.
 * 
 * @param index The index of the GbtValueBase object to retrieve.
 * @return A constant pointer to the GbtValueBase object, or nullptr if the index is out of range.
 */
GbtValueBase const* GbtData::getValue(uint8_t index) const
{
    if(index >= m_gbtValueCount)
    {
        return nullptr;
    }

    return m_gbtValues[index];
}

/**
 * @brief Parse the raw data and populate the GbtData object with GbtValueBase objects.
 * 
 * This method parses the raw data and creates GbtValueBase objects based on the data types encountered.
 * The parsed GbtValueBase objects are stored in the GbtData object for later access.
 * 
 * @param data A pointer to the raw data.
 * @param size The size of the raw data.
 * @return 0 if the parsing is successful, otherwise the amount of unknown identifiers encountered.
 */
int GbtData::parse(uint8_t const* data,  size_t const size)
{
    size_t pos = 0;
    uint8_t unknownIdentifierCount = 0;
    uint8_t pduArrayIdent = 0;
    uint8_t pduStructureIdent = 0;    
    uint8_t pduStructureCounter[MAX_STRUCTURE_NESTED];
    uint8_t pduArrayCounter[MAX_ARRAY_NESTED];

    pduStructureCounter[0] = 0;
    pduArrayCounter[0] = 0;

    MyLog::log("GBTDATA", "Parse GBT datablock with size %d", size);

    while(pos < size)
    {
        uint8_t identifier = data[pos];

        // long invoke and priority
        if(identifier == 0x0f && pos == 0)
        {
            // parse the long invoke and priority, pos will be incremented by the parse method
            m_longInvokedPriorityId.parse(data, pos, 0, 0);

            MyLog::log("GBTDATA", "Long invoke and priority ID: %d", m_longInvokedPriorityId.getValue());
        }
        
        // datetime
        else if(identifier == 0x0c)
        {
            // parse the date and time, pos will be incremented by the parse method
            m_dateAndTime.parse(data, pos, 0, 0);

            MyLog::log("GBTDATA", "Date and time: %d-%d-%d %d:%d:%d", m_dateAndTime.getYear(), m_dateAndTime.getMonth(), m_dateAndTime.getDay(), m_dateAndTime.getHour(), m_dateAndTime.getMinute(), m_dateAndTime.getSecond());
        }

        // structure
        else if(identifier == 0x02)
        {
            // Current structure count decreased, remains on the same identifier
            if(pduStructureCounter[pduStructureIdent] != 0)
            {
                pduStructureIdent++;
            }

            pduStructureCounter[pduStructureIdent] = data[pos + 1];

            // The array, if present, is decremented when a new structure is encountered.
            if(pduArrayCounter[pduArrayIdent] > 0)
            {
                pduArrayCounter[pduArrayIdent]--;
            }

            // move the position to the next identifier
            pos += 2;
        }

        // array
        else if(identifier == 0x01)
        {
            if(pduArrayCounter[pduArrayIdent] != 0)
            {
                pduArrayIdent++;
            }

            pduArrayCounter[pduArrayIdent] = data[pos + 1];

            // move the position to the next identifier
            pos += 2;
        }

        // unit16
        else if(identifier == 0x12)
        {
            // create a new GbtUint16 object
            auto* gbtUint16 = new GbtUint16();

            // parse the data, pos will be incremented by the parse method
            if(gbtUint16->parse(data, pos, pduStructureCounter[pduStructureIdent], pduArrayCounter[pduArrayIdent]))
            {
                // store the GbtUint16 object in the array
                m_gbtValues[m_gbtValueCount++] = gbtUint16;

                MyLog::log("GBTDATA", "GBT parse uint16 value %d", gbtUint16->getValue());
            }
            else    
            {   
                // delete the GbtUint16 object if the parsing failed
                delete gbtUint16;

                MyLog::log("GBTDATA", "GBT parse uint16 value failed");
            }

            // Current structure count decreased, remains on the same identifier
            if(pduStructureCounter[pduStructureIdent] != 0)
            {
                pduStructureCounter[pduStructureIdent]--;
            }
        }

        // octete string
        else if(identifier == 0x09)
        {
            // create a new GbtOctetString object
            auto* gbtOctetString = new GbtOctetString();

            // parse the data, pos will be incremented by the parse method
            if(gbtOctetString->parse(data, pos, pduStructureCounter[pduStructureIdent], pduArrayCounter[pduArrayIdent]))
            {
                // store the GbtOctetString object in the array
                m_gbtValues[m_gbtValueCount++] = gbtOctetString;

                MyLog::log("GBTDATA", "GBT parse octet string value %s", gbtOctetString->getStringValue());
            }
            else
            {
                // delete the GbtOctetString object if the parsing failed
                delete gbtOctetString;

                MyLog::log("GBTDATA", "GBT parse octet string value failed");
            }

            // Current structure count decreased, remains on the same identifier
            if(pduStructureCounter[pduStructureIdent] != 0)
            {
                pduStructureCounter[pduStructureIdent]--;
            }
        }

        // unit8
        else if(identifier == 0x0f)
        {
            // create a new GbtUint8 object
            auto* gbtUint8 = new GbtUint8();

            // parse the data, pos will be incremented by the parse method
            if(gbtUint8->parse(data, pos, pduStructureCounter[pduStructureIdent], pduArrayCounter[pduArrayIdent]))
            {
                // store the GbtUint8 object in the array
                m_gbtValues[m_gbtValueCount++] = gbtUint8;

                MyLog::log("GBTDATA", "GBT parse uint8 value %d", gbtUint8->getValue());
            }
            else
            {
                // delete the GbtUint8 object if the parsing failed
                delete gbtUint8;

                MyLog::log("GBTDATA", "GBT parse uint8 value failed");
            }

            // Current structure count decreased, remains on the same identifier
            if(pduStructureCounter[pduStructureIdent] != 0)
            {
                pduStructureCounter[pduStructureIdent]--;
            }
        }

        // unit32
        else if(identifier == 0x06)
        {
            // create a new GbtUint32 object
            auto* gbtUint32 = new GbtUint32();

            // parse the data, pos will be incremented by the parse method
            if(gbtUint32->parse(data, pos, pduStructureCounter[pduStructureIdent], pduArrayCounter[pduArrayIdent]))
            {
                // store the GbtUint32 object in the array
                m_gbtValues[m_gbtValueCount++] = gbtUint32;

                MyLog::log("GBTDATA", "GBT parse uint32 value %d", gbtUint32->getValue());
            }
            else
            {
                // delete the GbtUint32 object if the parsing failed
                delete gbtUint32;

                MyLog::log("GBTDATA", "GBT parse uint32 value failed");
            }

            // Current structure count decreased, remains on the same identifier
            if(pduStructureCounter[pduStructureIdent] != 0)
            {
                pduStructureCounter[pduStructureIdent]--;
            }
        }
        
        // default
        else {

            MyLog::log("GBTDATA", "GBT parse unknown data type %d", identifier);

            // move the position to the next identifier
            pos++;

            // count the number of unknown identifiers
            unknownIdentifierCount++;
        }
    }

    return unknownIdentifierCount;
}

