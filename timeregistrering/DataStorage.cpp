#include "DataStorage.h"
#include <Arduino.h>


void DataStorage::Initialize(int eeprom_size, uint16_t msg_index){
  // Serial.println("Initializing Data Storage...");
  EEPROM_SIZE = eeprom_size;  
  MESSAGES_BLOCKS_START = msg_index;
  //MESSAGES_CURRENT_INDEX = this->FindMessagesCurrentLocation();
  MSG_DATA_SIZE = sizeof(ButtonMsg);
  //_hotspotData = new WifiConfig("", HOTSPOT_SSID, HOTSPOT_PW);
  //_wifiData = new WifiConfig("", "", "");
}

WifiConfig DataStorage::GetHotspotConnectionData(){
  return _hotspotData;
}

WifiConfig DataStorage::GetWifiConnectionData(){
  this->readWifiData();
  return _wifiData;
}

void DataStorage::saveWifiData(WifiConfig config){
// Get the lengths of the username and password
  uint8_t ssid_len = config.ssid.length();
  uint8_t password_len = config.password.length();

  // Ensure lengths are within the maximum allowed limits
  if (ssid_len > SSID_MAX_LEN || password_len > PASSWORD_MAX_LEN) {
    // Serial.println("Error: Username or Password is too long");
    return;
  }

  // Write the length of the username to EEPROM at address 0
  EEPROM.write(0, ssid_len);

  // Write the username to EEPROM starting at address 1
  for (int i = 0; i < ssid_len; i++) {
    EEPROM.write(1 + i, config.ssid[i]);
  }

  // Write the length of the password to EEPROM after the username
  EEPROM.write(1 + ssid_len, password_len);

  // Write the password to EEPROM after the password length byte
  for (int i = 0; i < password_len; i++) {
    EEPROM.write(1 + ssid_len + 1 + i, config.password[i]);
  }
}

 WifiConfig DataStorage::readWifiData(){
  // Read the length of the ssid from EEPROM
  uint8_t ssid_len = EEPROM.read(0);
  // Serial.print("SSID_LEN");
  // Serial.println(ssid_len);
  if(ssid_len == 0xff){    
    return _wifiData;
  }

  // Ensure the length is within the expected range
  // if (ssid_len > SSID_MAX_LEN) {
  //   // Serial.println("Error: Invalid ssid length");
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
  //   // Serial.println("Error: Invalid password length");
  //   return;
  // }

  // Read the password from EEPROM
  char password[PASSWORD_MAX_LEN + 1];  // +1 for the null terminator
  for (int i = 0; i < password_len; i++) {
    password[i] = EEPROM.read(1 + ssid_len + 1 + i);
  }
  password[password_len] = '\0';  // Null-terminate the string

  String ssid_string(ssid);
  String pw_string(password);
  //_wifiData = new WifiConfig("", ssid, password);
  _wifiData.ssid = ssid_string;
  _wifiData.password = pw_string;
  // Serial.println("           ");
  // Serial.print("Saved SSID: ");
  // Serial.println(_wifiData.ssid);
  // Serial.print("Saved Password: ");
  // Serial.println(_wifiData.password);
  // Serial.println("           ");


  return _wifiData;

 }
 ////////////////////////////////////////////////////////
 ///////////      TIMESTAMP      ////////////////////////
 ////////////////////////////////////////////////////////

 void DataStorage::writeTimestamp(uint32_t timestamp) {
  // Calculate the number of blocks (4 bytes each)
  int write_address = TIMESTAMP_START_ADDRESS;
  int end_address = TIMESTAMP_START_ADDRESS + (TIMESTAMP_BLOCKS_COUNT * 4) - 1;
  // Serial.println(end_address);

  // Find the first non-FFFF block
  for (int i = 0; i < TIMESTAMP_BLOCKS_COUNT; i++) {
    uint16_t data = EEPROM.read(write_address) | (EEPROM.read(write_address + 1) << 8);
    if (data != 0xFFFF) {
      write_address += 4;  // +4 because we want the first byte of the NEXT block
      break; // we found it
    }
    write_address += 4;
  }

  // If we reached the end, wrap around to the beginning
  if (write_address >= end_address) {
    write_address = TIMESTAMP_START_ADDRESS;
  }

  // Invalidate the previous block
  int prev_address = write_address - 4;
  if (prev_address < TIMESTAMP_START_ADDRESS) {
        prev_address = end_address - 3;   // -3 because we want the FIRST byte of the last block. F.ex if last block ends is 120 121 122 123 
  } 

   for (int i = 0; i < 4; i++) {
      EEPROM.write(prev_address + i, 0xFF);
    }

// TEST


  // // Serial.print("W: ");
  // // Serial.print(write_address);
  // // Serial.print(" -> ");
  // // Serial.println(write_address + 3);

  // // Serial.print("P: ");
  // // Serial.print(prev_address);
  // // Serial.print(" -> ");
  // // Serial.println(prev_address + 3);





  // Write the new timestamp
  for (int i = 0; i < 4; i++) {
    EEPROM.write(write_address + i, (timestamp >> (i * 8)) & 0xFF);
  }


  // TEST
  // Serial.println("======    WRITE     =====");
  // Serial.println("");
  for(int b = 0; b < TIMESTAMP_BLOCKS_COUNT; b++){
     uint32_t timestamp = 0;
    for (int i = 0; i < 4; i++) {
      timestamp |= EEPROM.read(TIMESTAMP_START_ADDRESS + (b * 4) + i) << (i * 8);
    }

    // Serial.print("[ ");
    if(timestamp != 4294967295){
      // Serial.print(timestamp);
    } else {
      // Serial.print(" xxxxx ");
    }    
    // Serial.print(" ]");
  }

    // Serial.println("");
    // Serial.println("======    WRITE     =====");   
}

uint32_t DataStorage::readTimestamp() {

  int end_address = TIMESTAMP_START_ADDRESS + (TIMESTAMP_BLOCKS_COUNT * 4);
  // Find the first valid block
  int read_address = TIMESTAMP_START_ADDRESS;
  for (int i = 0; i < TIMESTAMP_BLOCKS_COUNT; i++) {
    uint16_t data = EEPROM.read(read_address) | (EEPROM.read(read_address + 1) << 8);
    if (data != 0xFFFF) {
      break;
    }
    read_address += 4;
  }

  // If no valid block is found, it means the section was never used, so we start at the beginning
  if (read_address > end_address) {
    read_address = TIMESTAMP_START_ADDRESS;
  }

  // Read the timestamp from the found block
  uint32_t timestamp = 0;
  for (int i = 0; i < 4; i++) {
    timestamp |= EEPROM.read(read_address + i) << (i * 8);
  }

  return timestamp;
}


 ////////////////////////////////////////////////////////
 ///////////      SNAKE_POINTERS      ////////////////////////
 ////////////////////////////////////////////////////////



void DataStorage::writeSnakePointer(SnakePointers _pointers) {
  // Serial.print("writePointers: Got pointers: ");
  // Serial.print("Start: ");
  // Serial.print(_pointers.start);
  // Serial.print("End: ");
  // Serial.println(_pointers.end);
  // Calculate the number of blocks (4 bytes each)
  int write_address = SNAKE_POINTERS_START_ADDRESS;
  int end_address = SNAKE_POINTERS_START_ADDRESS + (SNAKE_POINTERS_BLOCKS_COUNT * 4) - 1;
  // Serial.println(end_address);

  // Find the first non-FFFF block
  for (int i = 0; i < SNAKE_POINTERS_BLOCKS_COUNT; i++) {
    uint16_t data = EEPROM.read(write_address) | (EEPROM.read(write_address + 1) << 8);
    if (data != 0xFFFF) {
      write_address += 4;  // +4 because we want the first byte of the NEXT block
      break; // we found it
    }
    write_address += 4;
  }

  // If we reached the end, wrap around to the beginning
  if (write_address >= end_address) {
    write_address = SNAKE_POINTERS_START_ADDRESS;
  }

  // Invalidate the previous block
  int prev_address = write_address - 4;
  if (prev_address < SNAKE_POINTERS_START_ADDRESS) {
        prev_address = end_address - 3;   // -3 because we want the FIRST byte of the last block. F.ex if last block ends is 120 121 122 123 
  } 

   for (int i = 0; i < 4; i++) {
      EEPROM.write(prev_address + i, 0xFF);
    }

// TEST


  // Serial.print("W: ");
  // Serial.print(write_address);
  // Serial.print(" -> ");
  // Serial.println(write_address + 3);

  // Serial.print("P: ");
  // Serial.print(prev_address);
  // Serial.print(" -> ");
  // Serial.println(prev_address + 3);

  // Write the first 16-bit value
  EEPROM.write(write_address, _pointers.start & 0xFF);        // Write lower byte of value1
  EEPROM.write(write_address + 1, (_pointers.start >> 8) & 0xFF);    // Write upper byte of value1

  // Write the second 16-bit value
  EEPROM.write(write_address + 2, _pointers.end & 0xFF);    // Write lower byte of value2
  EEPROM.write(write_address + 3, (_pointers.end >> 8) & 0xFF);    // Write upper byte of value2

  // TEST
  // Serial.println("======    WRITE     =====");
  // Serial.println("");

  SnakePointers _p;

  for(int b = 0; b < SNAKE_POINTERS_BLOCKS_COUNT; b++){
        _p.start = EEPROM.read(SNAKE_POINTERS_START_ADDRESS + (b * 4)) | (EEPROM.read(SNAKE_POINTERS_START_ADDRESS + 1 + (b * 4) ) << 8);
        _p.end = EEPROM.read(SNAKE_POINTERS_START_ADDRESS + 2 + (b * 4)) | (EEPROM.read(SNAKE_POINTERS_START_ADDRESS + 3 + (b * 4)) << 8);
    
  

    // Serial.print("[ ");
    if(_p.start + _p.end != 131070){ //
      // Serial.print( _p.start);
      // Serial.print(", ");
      // Serial.print( _p.end);
    } else {
      // Serial.print(" xxxxx ");
    }    
    // Serial.print(" ]");
  }

    // Serial.println("");
    // Serial.println("======    WRITE     =====");   
    }

SnakePointers DataStorage::readSnakePointers() {
  SnakePointers _pointers;
  int end_address = SNAKE_POINTERS_START_ADDRESS + (SNAKE_POINTERS_BLOCKS_COUNT * 4);
  // Find the first valid block
  int read_address = SNAKE_POINTERS_START_ADDRESS;
  for (int i = 0; i < SNAKE_POINTERS_BLOCKS_COUNT; i++) {
    uint16_t data = EEPROM.read(read_address) | (EEPROM.read(read_address + 1) << 8);
    if (data != 0xFFFF) {      
      break;
    }
    read_address += 4;
  }

  // Serial.print("start address: ");
  // Serial.print(SNAKE_POINTERS_START_ADDRESS);

  // Serial.print(" - read_address: ");
  // Serial.print(read_address);
  // Serial.print(", end_address: ");
  // Serial.println(end_address);
  // If no valid block is found, it means the section was never used, so we start at the beginning
  if (read_address >= end_address) {
    // Serial.println("NO POINTER DATA FOUND");
    // 0 means no pointer OR snake data was found
    _pointers.start = 0;
    _pointers.end = 0;
    return _pointers;
  }

  // Serial.println("READ SNAKE POINTERS: Pointer data found!");
  // Serial.print("READ SNAKE POINTERS: read_address: ");
  // Serial.println(read_address);
  _pointers.start = EEPROM.read(read_address) | (EEPROM.read(read_address + 1) << 8);
  _pointers.end = EEPROM.read(read_address + 2) | (EEPROM.read(read_address + 3) << 8);

        //   _p.start = EEPROM.read(SNAKE_POINTERS_START_ADDRESS + (b * 4)) | (EEPROM.read(SNAKE_POINTERS_START_ADDRESS + 1 + (b * 4) ) << 8);
        // _p.end = EEPROM.read(SNAKE_POINTERS_START_ADDRESS + 2 + (b * 4)) | (EEPROM.read(SNAKE_POINTERS_START_ADDRESS + 3 + (b * 4)) << 8);

  return _pointers;
}

 ////////////////////////////////////////////////////////
 ///////////////      SNAKE      ////////////////////////
 ////////////////////////////////////////////////////////

ButtonSystemEvent DataStorage::popEventData() {
  
      // Serial.println("");
      // Serial.println("================= POP START ===============");
      // Serial.println("");
  SnakePointers _pointers = this->readSnakePointers();
  // Serial.print("Got pointers: Start: ");
  // Serial.print(_pointers.start);
  // Serial.print(", End: ");
  // Serial.println(_pointers.end);
  ButtonSystemEvent _event;

  if(_pointers.end == 0){
    _event.button_id = 0;
    _event.timestamp = 5555;
    return _event;
  }
  
  _event.button_id = EEPROM.read(_pointers.start);
  _event.timestamp = this->readUint32Eeprom(_pointers.start + 1);

   // Serial.println("Read Event: ");
  // Serial.print("Button ID: ");
  // Serial.print(_event.button_id);
  // Serial.print("Offset: ");
  // Serial.println(_event.timestamp);

  _pointers.start += 5;

  // This happens if we just popped the LAST node
  if(_pointers.start > _pointers.end){
    _pointers.start = 0;
    _pointers.end = 0;
  }

  this->writeSnakePointer(_pointers); 

  
      // Serial.println("");
      // Serial.println("================= POP END ===============");
      // Serial.println("");

  return _event;

}


ButtonSystemEvent DataStorage::peekEventData(){
  // Serial.println("");
  // Serial.println("================= PEEK START ===============");
  // Serial.println("");
  SnakePointers _pointers = this->readSnakePointers();
  // Serial.print("Got pointers: Start: ");
  // Serial.print(_pointers.start);
  // Serial.print(", End: ");
  // Serial.println(_pointers.end);
  ButtonSystemEvent _event;

  if(_pointers.end == 0){
    _event.button_id = 0;
    return _event;
  }
  
  _event.button_id = EEPROM.read(_pointers.start);
  _event.timestamp = this->readUint32Eeprom(_pointers.start + 1);

  // Serial.println("Read Event: ");
  // Serial.print("Button ID: ");
  // Serial.print(_event.button_id);
  // Serial.print("Offset: ");
  // Serial.println(_event.timestamp);

  // Serial.println("");
  // Serial.println("================= PEEK END ===============");
  // Serial.println("");

  return _event;

}

void DataStorage::writeEventData(ButtonSystemEvent _event) {
  Serial.print("Saving event. ");
  Serial.print("ButtonID: ");
  Serial.print(_event.button_id);
  SnakePointers _pointers = this->readSnakePointers();
  int write_address = 0;
  // Serial.print("writeEventData: read pointers");
    // Serial.print("Start: ");
  // Serial.print(_pointers.start);
  // Serial.print("End: ");
  // Serial.println(_pointers.end);
  // 0 means no data was present
  if(_pointers.end == 0){
    // Serial.println("GOT 0");
    _pointers.start = SNAKE_START_ADDRESS;
    _pointers.end = SNAKE_START_ADDRESS- 5; // We always write to the block AFTER pointers.end, so we manually set it to a previous one for code simplicity
  }

  write_address = _pointers.end + 5;

  EEPROM.write(write_address, _event.button_id);
  EEPROM.write(write_address + 1, _event.timestamp & 0xFF);        
  EEPROM.write(write_address + 2, (_event.timestamp >> 8) & 0xFF);    
  EEPROM.write(write_address + 3, (_event.timestamp >> 16) & 0xFF);    
  EEPROM.write(write_address + 4, (_event.timestamp >> 24) & 0xFF);    

  _pointers.end += 5;

  this->writeSnakePointer(_pointers);
}

// Utilities

uint32_t DataStorage::readUint32Eeprom(int address) {
  uint32_t value = 0;
  
  // Read 4 bytes from the EEPROM and construct the uint32_t
  value = EEPROM.read(address);
  value |= ((uint32_t)EEPROM.read(address + 1) << 8);
  value |= ((uint32_t)EEPROM.read(address + 2) << 16);
  value |= ((uint32_t)EEPROM.read(address + 3) << 24);
  
  return value;
}

uint16_t DataStorage::FindMessagesCurrentLocation() {
  // Serial.println("Finding messages index...");
  for (uint16_t i = MESSAGES_BLOCKS_START; i < EEPROM_SIZE; i++){
    if(EEPROM.read(i) != 0xFF){
      return i; // the first byte that is NOT set to 255 has to be the beginning of our used cell
    }
  }

  // If none is found, the eeprom has been reset, so we start from the default index
  return MESSAGES_BLOCKS_START;
}

void DataStorage::ResetEeprom() {
  // Serial.println("Resetting EEPROM");
   for (size_t i = 0; i < MESSAGES_BLOCKS_START; i++) {
        EEPROM.write(i, 0xff);
 }

 for (size_t i = MESSAGES_BLOCKS_START; i < EEPROM.length(); i++) {
        // Make sure you don't exceed the EEPROM size
        if (i < EEPROM.length()) {
            EEPROM.write(i, 0xFF);
        }
 }

  Serial.println("-- SETUP: EEPROM reset.");
}