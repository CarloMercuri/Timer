#include "WiFiTypes.h"
#include "NetComm.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include "serial_number.h"

NetComm::NetComm(){
}

void NetComm::Initialize(DataStorage* _data){
  this->_dataStorage = _data;

}

int NetComm::GetConnectionStatus() {
    if(WiFi.status() != WL_CONNECTED){
      this->connection_status = NO_CONNECTION;
    }

    return this->connection_status;
}

String NetComm::GetConnectionStatusFormatted(){
  int status = this->GetConnectionStatus();

  switch(status){
    case 0:
      return "NOT_CONNECTED";
    case 1:
      return "CONNECTED_HOTSPOT";
    case 2:
      return "CONNECTED_WIFI";
  }
}

bool NetComm::TryConnectWiFi() {  
    WifiConfig c = _dataStorage->GetWifiConnectionData();
    //Serial.println("Attempting to connect to Wifi.");
  if(c.ssid.length() == 0){
    // Serial.println("!!!!!!!!!!! Cannot connect to wifi. Wifi config is empty.");
    // Serial.println("    ");
    return false;
  }

  // Serial.println(c.ssid);
  // Serial.println(c.password);

  WiFi.begin(c.ssid.c_str(), c.password.c_str());

  // Wait for connection
  int attemptCounter = 1;
  while (WiFi.status() != WL_CONNECTED && attemptCounter < 3) {    
    delay(1000);
    attemptCounter++;
    // Serial.print("Attempt ");
    // Serial.println(attemptCounter);
  }

  // Check if connected to network
  if(WiFi.status() == WL_CONNECTED){
    //Serial.println("CONNECTED to wifi!");
    this->connection_status = CONNECTED_WIFI; // connected to wifi
    return true;
  } else {
    Serial.println("!!!!!!  Connection to wifi failed.");
    this->connection_status = NO_CONNECTION;
    return false;
  }
}

bool NetComm::TryConnectHotspot() {
  WifiConfig _hotSpotData = _dataStorage->GetHotspotConnectionData();
  // Serial.println("Attempting to connect to Hotspot.");
  //  Serial.println(_hotSpotData.ssid);
  // Serial.println(_hotSpotData.password);

  WiFi.begin(_hotSpotData.ssid.c_str(), _hotSpotData.password.c_str());

  // Wait for connection
  int attemptCounter = 1;
  while (WiFi.status() != WL_CONNECTED && attemptCounter < 11) {    
    delay(1000);
    attemptCounter++;
    // Serial.print("Attempt ");
    // Serial.println(attemptCounter);
  }

  // Check if connected to network
  if(WiFi.status() == WL_CONNECTED){
    //Serial.println("Connected to hotspot!");
    this->connection_status = CONNECTED_HOTSPOT; // connected to hotspot
    return true;
  } else {
    Serial.println("Connection to hotspot failed.");
    return false;
  }
}

uint32_t NetComm::SendUnixTimestampRequest() {
  // Serial.println("------------------  TIMESTAMP");
  // Serial.println("");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot send Timestamp request");
    return 0;
  }
    WiFiClient client;
    
    if (client.connect("192.168.1.36", 5001)) {  // Server port 80 for HTTP
    //Serial.println("TIMESTAMP: Connected to server");

    // Send HTTP GET request
    client.println("GET /api/Arduino/GetUnixTimestamp HTTP/1.1");
    client.println("Host: 192.168.1.36:5001"); // Replace with your server's host
    client.println("Connection: close");
    client.println();  // End of the request

    // Wait for a response from the server
    String responseBody = "";

    bool isBody = false;

    // Wait for a response from the server
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      // Serial.print("!!!!!!!!!!LINE: ");
      // Serial.println(line);
      line.trim();  // Remove any extra white space or line breaks

      // Check if we are at the start of the body
      if (line.length() == 0) {
        isBody = true;  // The next line is the start of the response body
        continue;
      }
       
       if (isBody) {
        if(line.charAt(0) == 'a' || line.charAt(0) == '0'){
          continue;
        }
        // Check if the line is a chunk size header
        // if (line.charAt(0) != '{' && line.charAt(0) != '[') {
        //   continue;  // Skip any lines that don't start with JSON data
        // }

        // This is the actual JSON body
        responseBody += line;
      }
    }


    // Close the connection
    client.stop();
    //Serial.println("TIMESTAMP: Disconnected from server.");

    uint32_t timestamp = responseBody.toInt();
    // Serial.print("Got new timestamp: body string: ");
    // Serial.println(responseBody);
    // Serial.print(" Parsed: ");
    // Serial.println(timestamp);
    return timestamp;
   
  } else {
    // Serial.println("Connection to server failed");
    return -1;
  }
}

WifiConfig NetComm::SendWifiDetailsRequest() {
  //Serial.println("Send GET");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot send GET request. Connection not active.");
    WifiConfig config("","","");
    return config;
  }
    WiFiClient client;
    
    if (client.connect("192.168.1.36", 5001)) {  // Server port 80 for HTTP

    // Send HTTP GET request
    client.println("GET /api/web/GetWifiConfig/"+ String(DEVICE_SERIAL_NUMBER) + " HTTP/1.1");
    client.println("Host: 192.168.1.36:5001"); // Replace with your server's host
    client.println("Connection: close");
    client.println();  // End of the request

    // Wait for a response from the server
    String responseBody = "";

    bool isBody = false;

    // Wait for a response from the server
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      line.trim();  // Remove any extra white space or line breaks

      // Check if we are at the start of the body
      if (line.length() == 0) {
        isBody = true;  // The next line is the start of the response body
        continue;
      }

       if (isBody) {
        // Check if the line is a chunk size header
        if (line.charAt(0) != '{' && line.charAt(0) != '[') {
          continue;  // Skip any lines that don't start with JSON data
        }

        // This is the actual JSON body
        responseBody += line;
      }
    }


    // Close the connection
    client.stop();
    //Serial.println("Disconnected from server.");


    // Parse the JSON response
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, responseBody);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      WifiConfig config("","","");
    return config;
    }

    // Convert JSON values to Strings first
    String serialNumberStr = doc["serialNumber"].as<String>();
    String ssidStr = doc["ssid"].as<String>();
    String passwordStr = doc["password"].as<String>();

    // Allocate memory for char* and copy the content
    WifiConfig config(strdup(serialNumberStr.c_str()), strdup(ssidStr.c_str()), strdup(passwordStr.c_str()));
    return config;
    // // Print the parsed data to Serial Monitor
    // Serial.println("Parsed Wifi Config:");
    // Serial.println("Serial Number: " + config.serialNumber);
    // Serial.println("SSID: " + config.ssid);
    // Serial.println("Password: " + config.password);
    //     // Create an instance of WifiConfig and populate it
   
  } else {
    // Serial.println("Connection to server failed");
    WifiConfig config("","","");
    return config;
  }
}


int NetComm::SendButtonEventRequest(int id , unsigned long long unixTimestamp) {
    if(GetConnectionStatus() == NO_CONNECTION){
      return -1;
    }
    WiFiClient client;
    Serial.print("Sending button event for button: ");
    Serial.println(id);
    
    if (client.connect("192.168.1.36", 5001)) {  
    //Serial.println("Connected to server");

    // Create a JSON object using ArduinoJson
        StaticJsonDocument<200> doc;
        doc["SerialNumber"] = DEVICE_SERIAL_NUMBER;
        doc["ButtonId"] = id;
        doc["UnixTimestamp"] = unixTimestamp;

        // Serialize JSON document to string
        String jsonBody;
        serializeJson(doc, jsonBody);

        // Print the JSON body for debugging
        //Serial.println("Body: " + jsonBody);

    // Send HTTP GET request
    client.println("POST /api/arduino/RegisterTime HTTP/1.1");
    client.println("Host: 192.168.1.36:5001"); // Replace with your server's host
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println("Content-Length: " + String(jsonBody.length()));
    client.println();  // End of the request
    client.println(jsonBody);  // JSON body
    
    // Wait for a response from the server
  int responseCode = -1;  // Initialize with an invalid code
  String responseBody = "";

  bool isBody = false;

  // Wait for a response from the server
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    line.trim();  // Remove any extra white space or line breaks

    // Check for the response status line (e.g., "HTTP/1.1 200 OK")
   if (line.startsWith("HTTP/")) {
      int firstSpaceIndex = line.indexOf(' ');
      int secondSpaceIndex = line.indexOf(' ', firstSpaceIndex + 1);
      if (firstSpaceIndex != -1 && secondSpaceIndex != -1) {
        String codeString = line.substring(firstSpaceIndex + 1, secondSpaceIndex);
        responseCode = codeString.toInt();  // Convert the status code to an integer
      }
      continue;  // Move to the next line
    }

    // Check if we are at the start of the body
    if (line.length() == 0) {
      isBody = true;  // The next line is the start of the response body
      continue;
    }

    if (isBody) {
      // We don't need to process the body, so we can just skip the lines
      continue;
    }
  }

    // Close the connection
    client.stop();
    // Serial.println("Disconnected from server.");

    // // Print the response code
    // Serial.print("Response Code: ");
    // Serial.println(responseCode);
    
    if (responseCode != 200) {
      Serial.println(responseCode);
      return -1;
      //  _mainLed.SetColor(255, 0, 0);
      // Do something if the response code is 200 (OK)
    } else {
      // Handle other response codes
        return responseCode;
      }
    }
}
