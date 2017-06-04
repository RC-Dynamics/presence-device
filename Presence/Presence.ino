/*
 * Copyright (c) 2016, CESAR.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 */

#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 4
#define RST_PIN 3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#include <KNoTThing.h>

#define RFID_SENSOR_ID     3
#define RFID_NAME   "RFID"

KNoTThing thing;

static int rfid_read(int32_t *val, int32_t *multiplier)
{

    *val = 0;
    *multiplier = 1;

    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      return 0;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
     return 0;
    }
   
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      *val = *val << 8;
      *val += mfrc522.uid.uidByte[i];
     }
    return 0;
}

static int rfid_write(int32_t *val, int32_t *multiplier)
{
    return 0;
}

void setup()
{
    Serial.begin(9600);
    
    
    thing.init("RFID");
    thing.registerIntData(RFID_NAME, RFID_SENSOR_ID, KNOT_TYPE_ID_NONE, KNOT_UNIT_NOT_APPLICABLE, rfid_read, rfid_write);
    mfrc522.PCD_Init();   // Inicia MFRC522
}

void loop()
{
    thing.run();
}
