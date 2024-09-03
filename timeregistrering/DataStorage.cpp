#include "DataStorage.h"
#include <Arduino.h>


void DataStorage::Initialize(int eeprom_size, uint16_t msg_index){
  Serial.println("Initializing Data Storage...");
  EEPROM_SIZE = eeprom_size;  
  MESSAGES_BLOCKS_START = msg_index;
  //MESSAGES_CURRENT_INDEX = this->FindMessagesCurrentLocation();
  MSG_DATA_SIZE = sizeof(ButtonMsg);
  _hotspotData = new WifiConfig("", HOTSPOT_SSID, HOTSPOT_PW);
  _wifiData = new WifiConfig("", "", "");
}

WifiConfig* DataStorage::GetHotspotConnectionData(){
  return this->_hotspotData;
}

WifiConfig* DataStorage::GetWifiConnectionData(){
  this->readWifiData();
  return _wifiData;
}

void DataStorage::saveWifiData(WifiConfig* config){
// Get the lengths of the username and password
  uint8_t ssid_len = strlen(config->ssid);
  uint8_t password_len = strlen(config->password);

  // Ensure lengths are within the maximum allowed limits
  if (ssid_len > SSID_MAX_LEN || password_len > PASSWORD_MAX_LEN) {
    Serial.println("Error: Username or Password is too long");
    return;
  }

  // Write the length of the username to EEPROM at address 0
  EEPROM.write(0, ssid_len);

  // Write the username to EEPROM starting at address 1
  for (int i = 0; i < ssid_len; i++) {
    EEPROM.write(1 + i, config->ssid[i]);
  }

  // Write the length of the password to EEPROM after the username
  EEPROM.write(1 + ssid_len, password_len);

  // Write the password to EEPROM after the password length byte
  for (int i = 0; i < password_len; i++) {
    EEPROM.write(1 + ssid_len + 1 + i, config->password[i]);
  }
}

 WifiConfig* DataStorage::readWifiData(){
  // Read the length of the ssid from EEPROM
  uint8_t ssid_len = EEPROM.read(0);
  Serial.print("SSID_LEN");
  Serial.println(ssid_len);
  if(ssid_len == 0xff){    
    return _wifiData;
  }

  // Ensure the length is within the expected range
  // if (ssid_len > SSID_MAX_LEN) {
  //   Serial.println("Error: Invalid ssid length");
  //   return null;
  // }

  // Read the ssid from EEPROM
  char ssid[SSID_MAX_LEN + 1];  // +1 for the null terminator

  for (int i = 0; i < ssid_len; i++) {
    ssid[i] = EEPROM.read(1 + i);
  }
  ssid[ssid_len] = '\0';  // Null-terminate the string

  // Read the length of the password from EEPROM
  uint8_t password_len = EEPROM.read(1 + ssid_len);

  // Ensure the length is within the expected range
  // if (password_len > PASSWORD_MAX_LEN) {
  //   Serial.println("Error: Invalid password length");
  //   return;
  // }

  // Read the password from EEPROM
  char password[PASSWORD_MAX_LEN + 1];  // +1 for the null terminator
  for (int i = 0; i < password_len; i++) {
    password[i] = EEPROM.read(1 + ssid_len + 1 + i);
  }
  password[password_len] = '\0';  // Null-terminate the string


  
  _wifiData->ssid = ssid;
  _wifiData->password = password;
  Serial.println("           ");
  Serial.print("Saved SSID: ");
  Serial.println(_wifiData->ssid);
  Serial.print("Saved Password: ");
  Serial.println(_wifiData->password);
  Serial.println("           ");


  return _wifiData;

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
  Serial.println("Resetting EEPROM");
   for (size_t i = 0; i < MESSAGES_BLOCKS_START; i++) {
        EEPROM.write(i, 0xff);
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