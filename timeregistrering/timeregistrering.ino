#include "NetComm.h"
#include "DataStorage.h"
#include "serial_number.h"
#include "LEDController.h"

DataStorage _data;
NetComm _comm;
LEDController _mainLed(2, 3, 4);

const uint16_t MESSAGES_DATA_INDEX = 80;

void setup() {
   Serial.begin(9600);
   _mainLed.SwitchOn();
  Serial.println("Initializing Arduino");
  // Initialize EEPROM with a size
  //EEPROM.begin(EEPROM.length());
  uint16_t eeprom_size = EEPROM.length();
  _data.Initialize(eeprom_size, MESSAGES_DATA_INDEX);
  _data.ResetEeprom();
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
  Serial.println("=============================");
  Serial.println("Connection status: ");

  Serial.println(_comm.GetConnectionStatusFormatted());
  Serial.println("=============================");
  Serial.println("                   ");

  switch(wifi_status){
    case NO_CONNECTION:
      HandleNoConnection();
      break;
    case CONNECTED_HOTSPOT:
      HandleConnectedHotspot();
      break;
    case CONNECTED_WIFI:
      HandleConnectedWifi();
      break;

    default:
    break;
  }

  delay(2000);

}

void HandleConnectedWifi(){
  _mainLed.SetColor(0, 255, 0);
    Serial.println("------- State: HandleConnectedWifi");
}

void HandleNoConnection() {
  _mainLed.SetColor(255, 0, 0);
  Serial.println("------- State: HandleNoConnection");
  // WifiConfig* fromEeprom = _data.readWifiData();

  if(!_comm.TryConnectWiFi()){
    _comm.TryConnectHotspot();
  }
}

void HandleConnectedHotspot() {
  _mainLed.SetColor(255, 255, 0);
    Serial.println("------- State: HandleConnectedHotspot");
  WifiConfig* fromEeprom = _data.readWifiData();

  if(strlen(fromEeprom->ssid) > 0){ // we have wifi saved locally
     _comm.TryConnectWiFi();
   
  } else {
    // We have no locally saved wifi data, but we are connected to the hotspot.
    // At this point we send a request to the server to see if the user has saved some
    // wifi credentials.
    WifiConfig c = _comm.SendWifiDetailsRequest();
    Serial.println("Fetched wifi information from server.");
    Serial.print("Serial Number: ");
    Serial.println(c.serialNumber);
    Serial.print("SSID: ");
    Serial.println(c.ssid);
    Serial.print("Password: ");
    Serial.println(c.password);

    if(strlen(c.ssid) == 0){
      return;
    }

    _data.saveWifiData(&c);
  }
}
