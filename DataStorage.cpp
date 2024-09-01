#include "DataStorage.h"
#include <Arduino.h>


void DataStorage::Initialize(int eeprom_size, uint16_t msg_index){
  Serial.println("Initializing Data Storage...");
  EEPROM_SIZE = eeprom_size;  
  MESSAGES_BLOCKS_START = msg_index;
  //MESSAGES_CURRENT_INDEX = this->FindMessagesCurrentLocation();
  MSG_DATA_SIZE = sizeof(ButtonMsg);
  _hotspotData = new WiFiConnectionData(HOTSPOT_SSID, HOTSPOT_PW);
}

WiFiConnectionData* DataStorage::GetHotspotConnectionData(){
  return this->_hotspotData;
}

void DataStorage::writeMessageData(const ButtonMsg &data) {
  // Wrap around if the address exceeds EEPROM size
  if (MESSAGES_CURRENT_INDEX + MSG_DATA_SIZE > EEPROM_SIZE) {
    MESSAGES_CURRENT_INDEX = MESSAGES_BLOCKS_START;
  }

  // Write the struct to EEPROM
  EEPROM.put(MESSAGES_CURRENT_INDEX, data);

  // Increment the index and save it
  MESSAGES_CURRENT_INDEX += MSG_DATA_SIZE;  
}

uint16_t DataStorage::FindMessagesCurrentLocation() {
  Serial.println("Finding messages index...");
  for (uint16_t i = MESSAGES_BLOCKS_START; i < EEPROM_SIZE; i++){
    if(EEPROM.read(i) != 0xFF){
      return i; // the first byte that is NOT set to 255 has to be the beginning of our used cell
    }
  }

  // If none is found, the eeprom has been reset, so we start from the default index
  return MESSAGES_BLOCKS_START;
}

void DataStorage::ResetEeprom() {
   for (size_t i = 0; i < MESSAGES_BLOCKS_START; i++) {
        EEPROM.write(i, 0);
 }

 for (size_t i = MESSAGES_BLOCKS_START; i < EEPROM.length(); i++) {
        // Make sure you don't exceed the EEPROM size
        if (i < EEPROM.length()) {
            EEPROM.write(i, 0xFF);
        }
 }

 Serial.println("EEPROM reset. Test:");
 Serial.println("Position 3: ");
Serial.print(EEPROM.read(3));

 Serial.println("Position 90: ");
Serial.print(EEPROM.read(90));
}