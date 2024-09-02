#pragma once
#include <cstdint>
 #include <stdint.h>
 #include <EEPROM.h>
 #include "hotspot_secrets.h"
 #include "wifi_config.h"

struct ButtonMsg {
  int button_id;
  char timestamp[19];
};

struct WiFiConnectionData {
  char* ssid;  
  char* password;

  WiFiConnectionData(char* ssid, char* password) : ssid(ssid), password(password) {} 
};

class DataStorage
{
  public:
    void Initialize(int eeprom_size, uint16_t messages_index);
    void writeMessageData(const ButtonMsg &data);
    void saveWifiData(WifiConfig* config);
    WifiConfig readWifiData();
    void ResetEeprom();
    uint16_t FindMessagesCurrentLocation();
    ButtonMsg readMessage();
    WiFiConnectionData* GetHotspotConnectionData();
    // WiFiConnectionData* GetWifiConnectionData();
    
  private:
    uint16_t EEPROM_SIZE; // Replace with the size of your EEPROM
    int INDEX_ADDRESS;  // The EEPROM address where we store the current write index
    uint16_t MESSAGES_BLOCKS_START;
    uint16_t MESSAGES_CURRENT_INDEX;
    size_t MSG_DATA_SIZE;
    int DATA_SIZE; // Size of the struct
    WiFiConnectionData *_wifiData;
    WiFiConnectionData *_hotspotData;
};

