#pragma once
#include <cstdint>
 #include <stdint.h>
 #include <EEPROM.h>
 #include "hotspot_secrets.h"
 #include "wifi_config.h"

 #define TIMESTAMP_START_ADDRESS 100
#define TIMESTAMP_BLOCKS_COUNT 6
#define SNAKE_POINTERS_START_ADDRESS TIMESTAMP_START_ADDRESS + (TIMESTAMP_BLOCKS_COUNT * 4)
#define SNAKE_POINTERS_BLOCKS_COUNT 20

#define SNAKE_START_ADDRESS SNAKE_POINTERS_START_ADDRESS + (SNAKE_POINTERS_BLOCKS_COUNT * 4) + 50
#define SNAKE_END_ADDRESS 3000

struct ButtonMsg {
  int button_id;
  int unix_timestamp;
};

struct ButtonSystemEvent {
  byte button_id;
  uint16_t time_offset;
};

struct SnakePointers {
  uint16_t start;
  uint16_t end;
};


class DataStorage
{
  public:
    
    void Initialize(int eeprom_size, uint16_t messages_index);
    void saveWifiData(WifiConfig config);
    WifiConfig readWifiData();
    void ResetEeprom();
    uint16_t FindMessagesCurrentLocation();

    // Timestamp
    uint32_t readTimestamp();
    void writeTimestamp(uint32_t timestamp);

    // Snake pointers
    void writeSnakePointer(SnakePointers pointers);
    SnakePointers readSnakePointers();

    // Events
    void writeEventData(ButtonSystemEvent _event);

    /**
    * Returns the first event in the system memory's queue. Button id is -1 if no event is present
    * 
    */
    ButtonSystemEvent peekEventData();

  /**
    * Returns the first event in the system memory's queue and deletes that event from the queue. Button id is -1 if no event is present
    * 
    */
    ButtonSystemEvent popEventData();

    ButtonMsg readMessage();
    WifiConfig GetHotspotConnectionData();
    WifiConfig GetWifiConnectionData();
    
  private:
    uint16_t EEPROM_SIZE; // Replace with the size of your EEPROM
    int INDEX_ADDRESS;  // The EEPROM address where we store the current write index
    uint16_t MESSAGES_BLOCKS_START;
    uint16_t MESSAGES_CURRENT_INDEX;
    size_t MSG_DATA_SIZE;
    int DATA_SIZE; // Size of the struct
    WifiConfig _wifiData{"", "", ""};
    WifiConfig _hotspotData{"", HOTSPOT_SSID, HOTSPOT_PW};
};



/*

                STRUCTURE

  [ 0 - 99 ] = wifi ssid/password
  [100 - 123 ] = 4 bytes reference unix timestamp times 6, shifting
  [124 - 203 ] = 2 bytes uint start_pointer, 2 bytes uint end_pointer. shifting
  [250 - 3000] = data: 3 bytes [b, b, b, b, b ,b, b, b][b, b, b, b, b ,b, b, b][b, b, b, b, b ,b, b, b]
                               |______________________||______________________________________________|
                                      Button ID                           Time offset
    Time offset is usable for 18 hours, so refresh every 17

*/

