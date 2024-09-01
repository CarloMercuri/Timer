#include "WiFiTypes.h"
#include "NetComm.h"
#include <Arduino.h>


void NetComm::Initialize(DataStorage* _data){
  this->_dataStorage = _data;

}

int NetComm::GetConnectionStatus() {
    if(WiFi.status() != WL_CONNECTED){
      this->connection_status = NO_CONNECTION;
    }

    return this->connection_status;
}

bool NetComm::TryConnectHotspot() {
  WiFiConnectionData* _hotSpotData = _dataStorage->GetHotspotConnectionData();
  Serial.println("Attempting to connect to Hotspot.");
  WiFi.begin(_hotSpotData->ssid, _hotSpotData->password);

  // Wait for connection
  int attemptCounter = 1;
  while (WiFi.status() != WL_CONNECTED && attemptCounter < 11) {    
    delay(1000);
    Serial.print(".");
    attemptCounter++;
    Serial.print("Attempt ");
    Serial.print(attemptCounter);
  }

  // Check if connected to network
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Connected to hotspot!");
    this->connection_status = CONNECTED_HOTSPOT; // connected to hotspot
    return true;
  } else {
    Serial.println("Connection to hotspot failed.");
    return false;
  }
}

void SendWifiDetailsRequest() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot send GET request. Connection not active.");
    return;
  }
    HTTPClient http;

    String serverPath = "http://yourapi.com/endpoint";  // Replace with your API URL
    http.begin(serverPath);

    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println("Response Code: 200");
      Serial.println("Response payload: " + payload);

      // Parse the JSON object
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("JSON deserialization failed: ");
        Serial.println(error.c_str());
        return;
      }

      const char* newSSID = doc["ssid"];
      const char* newPassword = doc["password"];

      Serial.println("Received SSID: " + String(newSSID));
      Serial.println("Received Password: " + String(newPassword));

      // Optionally, you can now do something with the new SSID and Password
      // For example, reconnect to the new WiFi network, etc.

    } else if (httpResponseCode == 500) {
      Serial.println("Response Code: 500 - Server error");
      // Handle the server error as needed
    } else {
      Serial.print("Error on HTTP request. Response Code: ");
      Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();
  

}


