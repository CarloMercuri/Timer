#include "NetComm.h"
#include "DataStorage.h"
#include "serial_number.h"
#include "LEDController.h"

#define BTN1_PIN 8
#define BTN2_PIN 9
#define BTN3_PIN 12
#define BTN4_PIN 13

bool pressed1 = false;
bool pressed2 = false;
bool pressed3 = false;
bool pressed4 = false;

DataStorage _data;
NetComm _comm;
LEDController _mainLed(2, 3, 4);

const uint16_t MESSAGES_DATA_INDEX = 80;

void setup() {
   Serial.begin(9600);
   _mainLed.SwitchOn();

  Serial.println("Initializing Arduino");
  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);
  pinMode(BTN3_PIN, INPUT);
  pinMode(BTN4_PIN, INPUT);
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

  // switch(wifi_status){
  //   case NO_CONNECTION:
  //     HandleNoConnection();
  //     break;
  //   case CONNECTED_HOTSPOT:
  //     HandleConnectedHotspot();
  //     break;
  //   case CONNECTED_WIFI:
  //     HandleConnectedWifi();
  //     break;

  //   default:
  //   break;
  // } 

  if(digitalRead(BTN1_PIN)){
    if(!pressed1){
      ButtonSystemEvent _event;
      _event.button_id = 1;
      _event.time_offset = 3432;
      _data.writeEventData(_event);
      pressed1 = true;
    }
  } else {
    pressed1 = false;
  }

  if(digitalRead(BTN2_PIN)){
    if(!pressed2){
      ButtonSystemEvent _event;
      _event.button_id = 2;
      _event.time_offset = 44245;
      _data.writeEventData(_event);
      pressed2 = true;
    }
  } else {
    pressed2 = false;
  }

  if(digitalRead(BTN3_PIN)){
    if(!pressed3){
      ButtonSystemEvent _event;
      _event.button_id = 3;
      _event.time_offset = 1441432;
      _data.writeEventData(_event);
      pressed3 = true;
    }
  } else {
    pressed3 = false;
  }

  if(digitalRead(BTN4_PIN)){
    if(!pressed4){
      ButtonSystemEvent peek = _data.peekEventData();
      ButtonSystemEvent pop = _data.popEventData();    

      Serial.print("PEEK: ");
      Serial.print(" btn_id: ");
      Serial.print(peek.button_id);
        
      Serial.print(", offset: ");
      Serial.println(peek.time_offset);

      Serial.print("POP: ");
      Serial.print(" btn_id: ");
      Serial.print(pop.button_id);
        
      Serial.print(", offset: ");
      Serial.println(pop.time_offset);


      pressed4 = true;
    }
  } else {
    pressed4 = false;
  }


}

void HandleConnectedWifi(){
  _mainLed.SetColor(0, 255, 0);
    Serial.println("------- State: HandleConnectedWifi");
}

void HandleNoConnection() {
  _mainLed.SetColor(255, 0, 0);
  Serial.println("------- State: HandleNoConnection");
  // WifiConfig fromEeprom = _data.readWifiData();

  if(!_comm.TryConnectWiFi()){
    _comm.TryConnectHotspot();
  }
}

void HandleConnectedHotspot() {
  _mainLed.SetColor(255, 255, 0);
    Serial.println("------- State: HandleConnectedHotspot");
  WifiConfig fromEeprom = _data.readWifiData();

  if(fromEeprom.ssid.length() > 0){ // we have wifi saved locally
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

    if(c.ssid.length() == 0){
      return;
    }

    _data.saveWifiData(c);
  }
}
