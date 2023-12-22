/**
 * @file gbtvaluebase.cpp
 * @brief Implementation of the GbtValueBase class.
 * 
 * @version 1.0
 * @author MFA Informatik AG, Andreas Schneider
 */
#include <string.h>

#include "gbtvaluebase.h"

GbtValueBase::~GbtValueBase()
{

}

uint8_t GbtValueBase::getStructureIdent() const 
{
    return m_structureIdent;
}

uint8_t GbtValueBase::getArrayIdent() const 
{
    return m_arrayIdent;
}





