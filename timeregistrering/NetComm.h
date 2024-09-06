#include <WiFi.h>
#include "DataStorage.h"
#include <HttpClient.h>
#include <ArduinoJson.h>
#include "server_secrets.h"
#include "wifi_config.h"

#define NO_CONNECTION 0
#define CONNECTED_HOTSPOT 1
#define CONNECTED_WIFI 2

class NetComm {
public:
  /**
    * Constructor

    */
  NetComm();
  void Initialize(DataStorage* _data);
  int GetConnectionStatus();
  String GetConnectionStatusFormatted();
  bool TryConnectWiFi();
  bool TryConnectHotspot();
  WifiConfig SendWifiDetailsRequest();
  void SendButtonEventRequest(int buttonId, unsigned long long unixTimestamp);

private:
  int connection_status = NO_CONNECTION;  // 0 = not connected, 1 = connected to hotspot, 2 = connected to custom wifi
  DataStorage* _dataStorage;
  // Number of milliseconds to wait without receiving any data before we give up
  int kNetworkTimeout = 5 * 1000;
  // Number of milliseconds to wait if no data is available before trying again
  int kNetworkDelay = 1000;
};