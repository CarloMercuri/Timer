#include "NetComm.h"
#include "DataStorage.h"
#include "serial_number.h"
#include "LEDController.h"
#include "ButtonController.h"

DataStorage _data;
NetComm _comm;
LEDController _mainLed(2, 3, 4);
ButtonController _input(&_data, BTN1_PIN, BTN2_PIN, BTN3_PIN, BTN4_PIN);

uint32_t timestamp = 0;
uint32_t timestamp_system_ms = 0;

int saved_status = 0;

const uint16_t MESSAGES_DATA_INDEX = 80;

void setup() {
   Serial.begin(9600);
   _mainLed.SwitchOn();

  Serial.println("-- SETUP: Initializing Arduino");
  // Initialize EEPROM with a size
  //EEPROM.begin(EEPROM.length());
  uint16_t eeprom_size = EEPROM.length();
  _data.Initialize(eeprom_size, MESSAGES_DATA_INDEX);
  _data.ResetEeprom();
  _comm.Initialize(&_data);

  Serial.println("-- SETUP:  Arduino Initialized");
}



unsigned long lastWifiEventRun = 0;   // Track last time the WiFi event ran
unsigned long wifiEventDelay = 3000;  // Set the desired delay for WiFi events

void loop() {
    _input.CheckInputs();
    delay(2);
        HandleButtonPresses();
    // Get the current time
    unsigned long currentMillis = millis();
    
    // Non-blocking delay to replace delay(3000)
    if (currentMillis - lastWifiEventRun > wifiEventDelay) {
        lastWifiEventRun = currentMillis;  // Update the last run time
        
        // Run your existing logic here (runs every 3000ms)
        int wifi_status = _comm.GetConnectionStatus();
        if(wifi_status != saved_status){
          if(saved_status == NO_CONNECTION){
            // Check if there are messages to send
            CheckSavedMessages();
          }
          Serial.println("=============================");
          Serial.print("  WIFI Status changed to: ");
          Serial.println(_comm.GetConnectionStatusFormatted());
          Serial.println("=============================");
          saved_status = wifi_status;
        }        

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



}

void CheckSavedMessages() {
  bool keepGoing = true;
  while(keepGoing){
    ButtonSystemEvent _event = _data.peekEventData();
    if(_event.button_id > 0){
      if(_comm.SendButtonEventRequest(_event.button_id, _event.timestamp) != 200){
        keepGoing = false; // if we still couldnt' send them, stop here 
        break;
      } else {
        // Success! we can now pop the event and go for the eventual next one
        _data.popEventData();
      }
    } else {
      keepGoing = false; // stop here
    }
  }
}

void HandleButtonPresses(){
  ButtonSystemEvent _event;

  int result = 0;
  if(_input.IsButtonPressed(1)){
    Serial.println("Button 1 press");
        _event.timestamp = GetCorrectTimestamp();
      _event.button_id = 1;
      result = _comm.SendButtonEventRequest(1, _event.timestamp);
      Serial.print("Button event: Got response: ");
      Serial.println(result);
      if(result != 200){
        _data.writeEventData(_event);
      }
    }

     if(_input.IsButtonPressed(2)){
      _event.button_id = 2;
        _event.timestamp = GetCorrectTimestamp();
      result = _comm.SendButtonEventRequest(2, _event.timestamp);
      if(result != 200){
        _data.writeEventData(_event);
      }
    }

     if(_input.IsButtonPressed(3)){
       _event.button_id = 3;
         _event.timestamp = GetCorrectTimestamp();
      result = _comm.SendButtonEventRequest(3, _event.timestamp);
      if(result != 200){
        _data.writeEventData(_event);
      }
    }

     if(_input.IsButtonPressed(4)){
       _event.button_id = 4;
         _event.timestamp = GetCorrectTimestamp();
      result = _comm.SendButtonEventRequest(4, _event.timestamp);
      if(result != 200){
        _data.writeEventData(_event);
      }
    }
}

uint32_t GetCorrectTimestamp(){
  return timestamp + ((millis() - timestamp_system_ms) / 1000) ;
}

void HandleConnectedWifi(){
  _mainLed.SetColor(0, 120, 0);
    if(timestamp == 0){
      timestamp = _comm.SendUnixTimestampRequest();
      timestamp_system_ms = millis();
    }

}

void HandleNoConnection() {
  _mainLed.SetColor(255, 0, 0);

  if(!_comm.TryConnectWiFi()){
    _comm.TryConnectHotspot();
  }
}

void HandleConnectedHotspot() {
  _mainLed.SetColor(0, 0, 255);
    if(timestamp == 0){
      timestamp = _comm.SendUnixTimestampRequest();
      timestamp_system_ms = millis();
    }
  WifiConfig fromEeprom = _data.readWifiData();

  if(fromEeprom.ssid.length() > 0){ // we have wifi saved locally
     _comm.TryConnectWiFi();
   
  } else {
    // We have no locally saved wifi data, but we are connected to the hotspot.
    // At this point we send a request to the server to see if the user has saved some
    // wifi credentials.
    WifiConfig c = _comm.SendWifiDetailsRequest();
    // Serial.println("Fetched wifi information from server.");
    // Serial.print("Serial Number: ");
    // Serial.println(c.serialNumber);
    // Serial.print("SSID: ");
    // Serial.println(c.ssid);
    // Serial.print("Password: ");
    // Serial.println(c.password);

    if(c.ssid.length() == 0){
      return;
    }

    _data.saveWifiData(c);
  }
}
