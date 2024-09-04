#include "NetComm.h"
#include "DataStorage.h"
#include "serial_number.h"
#include "LEDController.h"
#include "ButtonController.h"

DataStorage _data;
NetComm _comm;
LEDController _mainLed(2, 3, 4);
ButtonController _buttonPins(10, 11, 12, 13);

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

unsigned long lastWifiEventRun = 0;   // Track last time the WiFi event ran
unsigned long wifiEventDelay = 3000;  // Set the desired delay for WiFi events

void loop() {
    // Get the current time
    unsigned long currentMillis = millis();
    
    // Non-blocking delay to replace delay(3000)
    if (currentMillis - lastWifiEventRun > wifiEventDelay) {
        lastWifiEventRun = currentMillis;  // Update the last run time
        
        // Run your existing logic here (runs every 3000ms)
        int wifi_status = _comm.GetConnectionStatus();
        Serial.println("=============================");
        Serial.println("Connection status: ");
        Serial.println(_comm.GetConnectionStatusFormatted());
        Serial.println("=============================");
        Serial.println("                   ");

        switch (wifi_status) {
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
    }

    // Check for button presses
    if (digitalRead(10) == HIGH) {
        _buttonPins.Interrupt(1); // Check if button 1 is pressed
    }
    if (digitalRead(11) == HIGH) {
        _buttonPins.Interrupt(2); // Check if button 2 is pressed
    }
    if (digitalRead(12) == HIGH) {
        _buttonPins.Interrupt(3); // Check if button 3 is pressed
    }
    if (digitalRead(13) == HIGH) {
        _buttonPins.Interrupt(4); // Check if button 4 is pressed
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
