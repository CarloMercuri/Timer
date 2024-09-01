#include "NetComm.h"
#include "DataStorage.h"

DataStorage _data;
NetComm _comm;

const uint16_t MESSAGES_DATA_INDEX = 80;

void setup() {
   Serial.begin(9600);

  Serial.println("Initializing Arduino");
  // Initialize EEPROM with a size
  //EEPROM.begin(EEPROM.length());
  uint16_t eeprom_size = EEPROM.length();
  _data.Initialize(eeprom_size, MESSAGES_DATA_INDEX);
  _comm.Initialize(&_data);


//EEPROM.write(5, 12);

  //resetEeprom();

  // Example struct to write
  // MyData dataToWrite = {1, 3.14, "Arduino"};

  // // Write the struct to EEPROM
  // writeStructToEEPROM(dataToWrite);

  // // Read the struct back from EEPROM (assuming we want to read the first entry)
  // MyData dataRead = readStructFromEEPROM(0);

  // // Print the read data
  // Serial.print("ID: "); Serial.println(dataRead.id);
  // Serial.print("Value: "); Serial.println(dataRead.value);
  // Serial.print("Name: "); Serial.println(dataRead.name);

}

void loop() {
  int wifi_status = _comm.GetConnectionStatus();
  Serial.println("Connection status: ");
  Serial.print(wifi_status);

  if(wifi_status == NO_CONNECTION){
    _comm.TryConnectHotspot();
  }


  delay(2000);

}



// void writeStructToEEPROM(const MyData &data) {
//   // Read the current write index
//   int writeIndex = EEPROM.read(INDEX_ADDRESS);

//   // Calculate the EEPROM address to write the struct
//   int address = INDEX_ADDRESS + sizeof(int) + writeIndex * DATA_SIZE;

//   // Wrap around if the address exceeds EEPROM size
//   if (address + DATA_SIZE > EEPROM_SIZE) {
//     writeIndex = 0;
//     address = INDEX_ADDRESS + sizeof(int);
//   }

//   // Write the struct to EEPROM
//   EEPROM.put(address, data);

//   // Increment the index and save it
//   writeIndex++;
//   EEPROM.put(INDEX_ADDRESS, writeIndex);
// }

// MyData readStructFromEEPROM(int index) {
//   // Calculate the EEPROM address to read the struct
//   int address = INDEX_ADDRESS + sizeof(int) + index * DATA_SIZE;

//   // Create a temporary struct to hold the data
//   MyData data;

//   // Read the struct from EEPROM
//   EEPROM.get(address, data);

//   return data;
// }
