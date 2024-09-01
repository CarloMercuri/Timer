#include <WiFi.h>
#include "DataStorage.h"
#include <HTTPClient.h>
#include <ArduinoJson.h> 

#define NO_CONNECTION 0
#define CONNECTED_HOTSPOT 1
#define CONNECTED_WIFI 2

class NetComm
{
  public:
    void Initialize(DataStorage* _data);
    int GetConnectionStatus();
    bool TryConnectHotspot();
    void SendWifiDetailsRequest();
    
  private:
  int connection_status = NO_CONNECTION; // 0 = not connected, 1 = connected to hotspot, 2 = connected to custom wifi
  DataStorage* _dataStorage;
};

