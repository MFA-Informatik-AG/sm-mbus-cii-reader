#include "unity.h"

#include <string.h>

#include "test_hdlc.h"

#include "hdlc.h"
#include "dlms.h"
#include "gbtdata.h"

#define HDLC_ARRAY_SIZE  443

const uint8_t hdlcArray[HDLC_ARRAY_SIZE] = {
      0x7e, 0xa0, 0x84, 0xce, 0xff, 0x03, 0x13, 0x12, 
      0x8b, 0xe6, 0xe7, 0x00, 0xe0, 0x40, 0x00, 0x01, 
      0x00, 0x00, 0x70, 0x0f, 0x00, 0x02, 0x32, 0x24, 
      0x0c, 0x07, 0xe6, 0x0b, 0x0b, 0x05, 0x14, 0x25, 
      0x1e, 0xff, 0x80, 0x00, 0x00, 0x02, 0x0f, 0x01, 
      0x0f, 0x02, 0x04, 0x12, 0x00, 0x28, 0x09, 0x06, 
      0x00, 0x08, 0x19, 0x09, 0x00, 0xff, 0x0f, 0x02, 
      0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x28, 
      0x09, 0x06, 0x00, 0x08, 0x19, 0x09, 0x00, 0xff, 
      0x0f, 0x01, 0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 
      0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x60, 0x01, 
      0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 0x02, 
      0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 
      0x01, 0x07, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 
      0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 
      0x01, 0x00, 0x02, 0x07, 0x00, 0xff, 0x0f, 0x02, 
      0x12, 0x00, 0x00, 0xb3, 0xf2, 0x7e, 0x7e, 0xa0, 
      0x7d, 0xce, 0xff, 0x03, 0x13, 0xd0, 0x45, 0xe0, 
      0x40, 0x00, 0x02, 0x00, 0x00, 0x6c, 0x02, 0x04, 
      0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x01, 0x01, 
      0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
      0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 
      0x01, 0x02, 0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 
      0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 
      0x06, 0x01, 0x01, 0x05, 0x08, 0x00, 0xff, 0x0f, 
      0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 
      0x03, 0x09, 0x06, 0x01, 0x01, 0x06, 0x08, 0x00, 
      0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 
      0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x01, 0x07, 
      0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
      0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 
      0x01, 0x08, 0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 
      0x00, 0x00, 0xc1, 0x95, 0x7e, 0x7e, 0xa0, 0x84, 
      0xce, 0xff, 0x03, 0x13, 0x12, 0x8b, 0xe0, 0x40, 
      0x00, 0x03, 0x00, 0x00, 0x73, 0x02, 0x04, 0x12, 
      0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x82, 0x07, 
      0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 0x02, 
      0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 
      0x1f, 0x07, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 
      0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 
      0x01, 0x00, 0x33, 0x07, 0x00, 0xff, 0x0f, 0x02, 
      0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 
      0x09, 0x06, 0x01, 0x00, 0x47, 0x07, 0x00, 0xff, 
      0x0f, 0x02, 0x12, 0x00, 0x00, 0x09, 0x06, 0x00, 
      0x08, 0x19, 0x09, 0x00, 0xff, 0x09, 0x08, 0x36, 
      0x30, 0x32, 0x32, 0x32, 0x39, 0x39, 0x39, 0x06, 
      0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
      0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 
      0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 
      0x66, 0xaa, 0x7e, 0x7e, 0xa0, 0x2e, 0xce, 0xff, 
      0x03, 0x13, 0x7e, 0x2d, 0xe0, 0xc0, 0x00, 0x04, 
      0x00, 0x00, 0x1d, 0x06, 0x00, 0x00, 0x00, 0x01, 
      0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
      0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x12, 
      0x00, 0x00, 0x12, 0x00, 0x00, 0x12, 0x00, 0x00, 
      0x61, 0x14, 0x7e
};


#define GBT_MYARRAY_SIZE 364

const uint8_t gbtArray[GBT_MYARRAY_SIZE] = {
      0x0f, 0x00, 0x02, 0x32, 0x24, 0x0c, 0x07, 0xe6, 
      0x0b, 0x0b, 0x05, 0x14, 0x25, 0x1e, 0xff, 0x80, 
      0x00, 0x00, 0x02, 0x0f, 0x01, 0x0f, 0x02, 0x04, 
      0x12, 0x00, 0x28, 0x09, 0x06, 0x00, 0x08, 0x19, 
      0x09, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
      0x02, 0x04, 0x12, 0x00, 0x28, 0x09, 0x06, 0x00, 
      0x08, 0x19, 0x09, 0x00, 0xff, 0x0f, 0x01, 0x12, 
      0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 
      0x06, 0x00, 0x00, 0x60, 0x01, 0x00, 0xff, 0x0f, 
      0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 
      0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x07, 0x00, 
      0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 
      0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x02, 
      0x07, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
      0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 
      0x01, 0x01, 0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 
      0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 
      0x06, 0x01, 0x01, 0x02, 0x08, 0x00, 0xff, 0x0f, 
      0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 
      0x03, 0x09, 0x06, 0x01, 0x01, 0x05, 0x08, 0x00, 
      0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 
      0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x01, 0x06, 
      0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
      0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 
      0x01, 0x07, 0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 
      0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 
      0x06, 0x01, 0x01, 0x08, 0x08, 0x00, 0xff, 0x0f, 
      0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 
      0x03, 0x09, 0x06, 0x01, 0x00, 0x82, 0x07, 0x00, 
      0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 
      0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x1f, 
      0x07, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
      0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 
      0x00, 0x33, 0x07, 0x00, 0xff, 0x0f, 0x02, 0x12, 
      0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 
      0x06, 0x01, 0x00, 0x47, 0x07, 0x00, 0xff, 0x0f, 
      0x02, 0x12, 0x00, 0x00, 0x09, 0x06, 0x00, 0x08, 
      0x19, 0x09, 0x00, 0xff, 0x09, 0x08, 0x36, 0x30, 
      0x32, 0x32, 0x32, 0x39, 0x39, 0x39, 0x06, 0x00, 
      0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 
      0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
      0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 
      0x00, 0x00, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 
      0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 
      0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x12, 0x00, 
      0x00, 0x12, 0x00, 0x00
};


#define GBT_MYARRAY2_SIZE 364

const uint8_t gbtArray2[GBT_MYARRAY2_SIZE] = {
  0x0f, 0x00, 0x08, 0x3d, 0x65, 0x0c, 0x07, 0xe6, 
  0x0c, 0x1a, 0x01, 0x01, 0x1e, 0x23, 0xff, 0x80, 
  0x00, 0x00, 0x02, 0x0f, 0x01, 0x0f, 0x02, 0x04, 
  0x12, 0x00, 0x28, 0x09, 0x06, 0x00, 0x08, 0x19, 
  0x09, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
  0x02, 0x04, 0x12, 0x00, 0x28, 0x09, 0x06, 0x00, 
  0x08, 0x19, 0x09, 0x00, 0xff, 0x0f, 0x01, 0x12, 
  0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 
  0x06, 0x00, 0x00, 0x60, 0x01, 0x00, 0xff, 0x0f, 
  0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 
  0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x07, 0x00, 
  0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 
  0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x02, 
  0x07, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
  0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 
  0x01, 0x01, 0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 
  0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 
  0x06, 0x01, 0x01, 0x02, 0x08, 0x00, 0xff, 0x0f, 
  0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 
  0x03, 0x09, 0x06, 0x01, 0x01, 0x05, 0x08, 0x00, 
  0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 
  0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x01, 0x06, 
  0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
  0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 
  0x01, 0x07, 0x08, 0x00, 0xff, 0x0f, 0x02, 0x12, 
  0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 
  0x06, 0x01, 0x01, 0x08, 0x08, 0x00, 0xff, 0x0f, 
  0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 
  0x03, 0x09, 0x06, 0x01, 0x00, 0x82, 0x07, 0x00, 
  0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 0x02, 0x04, 
  0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x1f, 
  0x07, 0x00, 0xff, 0x0f, 0x02, 0x12, 0x00, 0x00, 
  0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 
  0x00, 0x33, 0x07, 0x00, 0xff, 0x0f, 0x02, 0x12, 
  0x00, 0x00, 0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 
  0x06, 0x01, 0x00, 0x47, 0x07, 0x00, 0xff, 0x0f, 
  0x02, 0x12, 0x00, 0x00, 0x09, 0x06, 0x00, 0x08, 
  0x19, 0x09, 0x00, 0xff, 0x09, 0x08, 0x36, 0x30, 
  0x32, 0x32, 0x32, 0x39, 0x39, 0x39, 0x06, 0x00, 
  0x00, 0x00, 0x13, 0x06, 0x00, 0x00, 0x00, 0x00, 
  0x06, 0x00, 0x00, 0x00, 0x95, 0x06, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 
  0x00, 0x00, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 
  0x00, 0x06, 0x00, 0x00, 0x00, 0x52, 0x06, 0x00, 
  0x00, 0x00, 0x0b, 0x12, 0x00, 0x0b, 0x12, 0x00, 
  0x00, 0x12, 0x00, 0x00
};

void gbt_frame_handler(uint8_t const* data, size_t length) {
    
    TEST_MESSAGE("GBT Frame Received");

    TEST_ASSERT_EQUAL_INT8_ARRAY(gbtArray, data, length);

    GbtData gbtData;

    if(gbtData.parse(data, length) != 0)
    {
      TEST_FAIL_MESSAGE("GBT DATA BLOCK PARSE FAILED");
    }

    uint8_t valueCount = gbtData.getValueCount();

    TEST_ASSERT_EQUAL_INT8(74, valueCount);

    auto const& dateTime = gbtData.getDateTime();

    TEST_ASSERT_EQUAL_INT8(11, dateTime.getDay());
    TEST_ASSERT_EQUAL_INT8(11, dateTime.getMonth());
    TEST_ASSERT_EQUAL_INT16(2022, dateTime.getYear());
    TEST_ASSERT_EQUAL_INT8(20, dateTime.getHour());
    TEST_ASSERT_EQUAL_INT8(37, dateTime.getMinute());
    TEST_ASSERT_EQUAL_INT8(30, dateTime.getSecond());

    char buff[128];

    for(uint8_t i=0; i < valueCount; i++)
    {
        gbtData.getValue(i)->asString(buff, 128);

        TEST_MESSAGE(buff);
    }

}

Gbt gbt(gbt_frame_handler);
Dlms dlms(gbt);

void hdlc_frame_handler(uint8_t const* data, size_t const length, bool const frameValid) {
    
    TEST_MESSAGE("HDLC Frame Received");

    uint8_t hdlcData[length];
    size_t hdlcDataLenght = length - 8;

    memcpy(hdlcData, data + 8, hdlcDataLenght);

    GbtBlock const* addedBlock;

    if(dlms.hdlcDataReceived(hdlcData, hdlcDataLenght, addedBlock))
    {
      TEST_MESSAGE("Add GBT Frame");
    }
    else
    {
      TEST_FAIL_MESSAGE("Failed to add GBT Frame");
    }
}

Hdlc hdlc(&hdlc_frame_handler);

void test_full_hdlc(void) 
{
    dlms.reset();

    for(size_t i = 0; i<HDLC_ARRAY_SIZE; i++)
    {
      hdlc.charReceiver(hdlcArray[i]);    
    }
}

void test_gbt_array2(void)
{
    GbtData gbtData;

    if(gbtData.parse(gbtArray2, GBT_MYARRAY2_SIZE) != 0)
    {
      TEST_FAIL_MESSAGE("GBT DATA BLOCK PARSE FAILED");
    }

    TEST_ASSERT_EQUAL_INT8(74, gbtData.getValueCount());

    char buff[1024];

    auto const& dateTime = gbtData.getDateTime();

    TEST_ASSERT_EQUAL_INT8(26, dateTime.getDay());
    TEST_ASSERT_EQUAL_INT8(12, dateTime.getMonth());
    TEST_ASSERT_EQUAL_INT16(2022, dateTime.getYear());
    TEST_ASSERT_EQUAL_INT8(01, dateTime.getHour());
    TEST_ASSERT_EQUAL_INT8(30, dateTime.getMinute());
    TEST_ASSERT_EQUAL_INT8(35, dateTime.getSecond());
}


