#include "WiFiTypes.h"
#include "NetComm.h"
#include <Arduino.h>
#include <WiFiClient.h>


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
    Serial.println("Attempting to connect to Wifi.");
  if(c.ssid.length() == 0){
    Serial.println("!!!!!!!!!!! Cannot connect to wifi. Wifi config is empty.");
    Serial.println("    ");
    return false;
  }

  Serial.println(c.ssid);
  Serial.println(c.password);

  WiFi.begin(c.ssid.c_str(), c.password.c_str());

  // Wait for connection
  int attemptCounter = 1;
  while (WiFi.status() != WL_CONNECTED && attemptCounter < 3) {    
    delay(1000);
    attemptCounter++;
    Serial.print("Attempt ");
    Serial.println(attemptCounter);
  }

  // Check if connected to network
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("CONNECTED to wifi!");
    this->connection_status = CONNECTED_WIFI; // connected to wifi
    return true;
  } else {
    Serial.println("!!!!!!  Connection to wifi failed.");
    this->connection_status = NO_CONNECTION;
    return false;
  }
}

// bool NetComm::TryConnectWiFi(WifiConfig* c) {
//     Serial.println("Attempting to connect to Wifi.");
//   if(strlen(c->ssid) == 0){
//     Serial.println("!!!!!!!!!!! Cannot connect to wifi. Wifi config is empty.");
//     Serial.println("    ");
//     return false;
//   }

//   Serial.println(c->ssid);
//   Serial.println(c->password);

//   WiFi.begin(c->ssid, c->password);

//   // Wait for connection
//   int attemptCounter = 1;
//   while (WiFi.status() != WL_CONNECTED && attemptCounter < 3) {    
//     delay(1000);
//     attemptCounter++;
//     Serial.print("Attempt ");
//     Serial.println(attemptCounter);
//   }

//   // Check if connected to network
//   if(WiFi.status() == WL_CONNECTED){
//     Serial.println("CONNECTED to wifi!");
//     this->connection_status = CONNECTED_WIFI; // connected to wifi
//     return true;
//   } else {
//     Serial.println("!!!!!!  Connection to wifi failed.");
//     this->connection_status = NO_CONNECTION;
//     return false;
//   }
// }

bool NetComm::TryConnectHotspot() {
  WifiConfig _hotSpotData = _dataStorage->GetHotspotConnectionData();
  Serial.println("Attempting to connect to Hotspot.");
   Serial.println(_hotSpotData.ssid);
  Serial.println(_hotSpotData.password);

  WiFi.begin(_hotSpotData.ssid.c_str(), _hotSpotData.password.c_str());

  // Wait for connection
  int attemptCounter = 1;
  while (WiFi.status() != WL_CONNECTED && attemptCounter < 11) {    
    delay(1000);
    attemptCounter++;
    Serial.print("Attempt ");
    Serial.println(attemptCounter);
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

WifiConfig NetComm::SendWifiDetailsRequest() {
  Serial.println("Send GET");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot send GET request. Connection not active.");
    WifiConfig config("","","");
    return config;
  }
    WiFiClient client;
    
    if (client.connect("192.168.1.36", 5001)) {  // Server port 80 for HTTP
    Serial.println("Connected to server");

    // Send HTTP GET request
    client.println("GET /api/web/GetWifiConfig/KUF-XEI-4D35 HTTP/1.1");
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
    Serial.println("Disconnected from server.");


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
    Serial.println("Connection to server failed");
    WifiConfig config("","","");
    return config;
  }
  

}













// void NetComm::SendWifiDetailsRequest() {
//   Serial.println("Send GET");
//   if (WiFi.status() != WL_CONNECTED) {
//     Serial.println("Cannot send GET request. Connection not active.");
//     return;
//   }
//     WiFi w;
//     HttpClient http(w);

//     //http.begin(serverPath);

//     int httpResponseCode = 0;

//     httpResponseCode = http.get(SERVER_BASE_ADDRESS, API_ENDPOINT_UNIXTIME);
//     Serial.println("Response code: ");
//     Serial.print(httpResponseCode);
//     httpResponseCode = http.skipResponseHeaders();
//       if (httpResponseCode >= 0)
//       {
//         int bodyLen = http.contentLength();
//         Serial.print("Content length is: ");
//         Serial.println(bodyLen);
//         Serial.println();
//         Serial.println("Body returned follows:");
      
//         // Now we've got to the body, so we can print it out
//         unsigned long timeoutStart = millis();
//         char c;
//         // Whilst we haven't timed out & haven't reached the end of the body
//         while ( (http.connected() || http.available()) &&
//                ((millis() - timeoutStart) < this->kNetworkTimeout) )
//         {
//             if (http.available())
//             {
//                 c = http.read();
//                 // Print out this character
//                 Serial.print(c);
               
//                 bodyLen--;
//                 // We read something, reset the timeout counter
//                 timeoutStart = millis();
//             }
//             else
//             {
//                 // We haven't got any data, so let's pause to allow some to
//                 // arrive
//                 delay(this->kNetworkDelay);
//             }
//         }
//       }




//     // if (httpResponseCode == 200) {
//     //   String payload = http.getString();
//     //   Serial.println("Response Code: 200");
//     //   Serial.println("Response payload: " + payload);

//     //   // Parse the JSON object
//     //   StaticJsonDocument<200> doc;
//     //   DeserializationError error = deserializeJson(doc, payload);

//     //   if (error) {
//     //     Serial.print("JSON deserialization failed: ");
//     //     Serial.println(error.c_str());
//     //     return;
//     //   }

//     //   const char* newSSID = doc["ssid"];
//     //   const char* newPassword = doc["password"];

//     //   Serial.println("Received SSID: " + String(newSSID));
//     //   Serial.println("Received Password: " + String(newPassword));

//     //   // Optionally, you can now do something with the new SSID and Password
//     //   // For example, reconnect to the new WiFi network, etc.

//     // } else if (httpResponseCode == 500) {
//     //   Serial.println("Response Code: 500 - Server error");
//     //   // Handle the server error as needed
//     // } else {
//     //   Serial.print("Error on HTTP request. Response Code: ");
//     //   Serial.println(httpResponseCode);
//     // }

//     // // Free resources
//     // http.end();


// }


