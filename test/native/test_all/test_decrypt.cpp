#include "unity.h"

#include <Crypto.h>
#include <AES.h>
#include <GCM.h>
#include <string.h>

GCM<AES128> aes128;


#define MYARRAY_SIZE 98

const uint8_t encryptedAdpu[MYARRAY_SIZE] = {
    0xdb, 0x08, 0x4c, 0x47, 0x5a, 0x00, 0x00, 0xbc, 
    0x61, 0x4e, 0x57, 0x30, 0x00, 0x00, 0x00, 0x8c, 
    0xae, 0xf7, 0xf4, 0x55, 0x73, 0xee, 0x8a, 0x53, 
    0xa2, 0xe1, 0xa0, 0xe3, 0x17, 0xa8, 0x23, 0xc2, 
    0xdd, 0x04, 0xf4, 0xa6, 0x11, 0x88, 0x02, 0x1c, 
    0xcd, 0xeb, 0x55, 0x35, 0xfc, 0x48, 0xa1, 0x92, 
    0x73, 0xf3, 0x4b, 0x9d, 0xf0, 0xfc, 0x58, 0xf8, 
    0xb4, 0xab, 0x23, 0xf0, 0x6a, 0xde, 0x62, 0xa4, 
    0xf2, 0x15, 0x59, 0xae, 0x5d, 0x60, 0xa0, 0x54, 
    0xae, 0x0a, 0x4a, 0x1a, 0x2b, 0x4f, 0x08, 0xa9, 
    0xec, 0xbd, 0x82, 0xf8, 0xc2, 0x74, 0x8b, 0xfc, 
    0x8d, 0x07, 0xf8, 0xe4, 0xbf, 0xeb, 0x4c, 0x3a, 
    0xe4, 0xc2
};

// security byte (1st byte) and authentication key (16 bytes)
// security byte (1st byte) is set from dlms pdu 
uint8_t authenticationKey[17] = { 0x00, 0xd0, 0xd1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF};

uint8_t aes_key[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

uint8_t aes_iv[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void test_decript_hdlc(void) 
{
    // iv for cbc 12 bytes
    uint8_t iv[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // pdu, 1st byte must start with DB as GeneralGloCiphering command
    if(encryptedAdpu[0] != 0xdb)
    {
        TEST_ABORT();

        return;
    }
    
    // pdu, 2nd byte ist the lenght of the system title
    uint8_t systemTitleLenght = encryptedAdpu[1];

    // construct iv from system title and frame counter
    memcpy(&iv[0], &encryptedAdpu[2], systemTitleLenght);

    // frame counter is type 0x30 (uint32)
    memcpy(&iv[8], &encryptedAdpu[2 + systemTitleLenght + 2], 4);

    // security byte
    authenticationKey[0] = encryptedAdpu[11];

    // construct authentication tag, last 12 bytes 
    uint8_t tag[12];

    memcpy(&tag[0], &encryptedAdpu[sizeof(encryptedAdpu)-sizeof(tag)], sizeof(tag));
    
    aes128.setKey(aes_key, sizeof(aes_key));
    aes128.setIV(iv, sizeof(iv));

    aes128.addAuthData(authenticationKey, sizeof(authenticationKey));

    uint8_t encryptedMessageSize = sizeof(encryptedAdpu) - sizeof(tag) - 16;

    uint8_t encrypted[sizeof(encryptedAdpu)];

    // decrypted should be allways lower in size than encrypted
    uint8_t plainText[encryptedMessageSize];

    // cypertext start from the 16 byte (1 (GeneralGloCiphering) + 1 (system title lenght) + 8 (system title) + 1 (security byte) + 4 (frame counter) + 1 (next byte))
    memcpy(encrypted, &encryptedAdpu[16], encryptedMessageSize);

    aes128.decrypt(plainText, encrypted, encryptedMessageSize);

    TEST_ASSERT_TRUE_MESSAGE(aes128.checkTag(tag, sizeof(tag)), "Landis: DecryptAndExtractValues: Encryption Tag Check error");
}


