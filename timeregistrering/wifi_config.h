#pragma once

struct WifiConfig {
  char* ssid;
  char* password;
  char* serialNumber;

  WifiConfig(char* serialNumber, char* ssid, char* password) : serialNumber(serialNumber), ssid(ssid), password(password) {} 
};

#define SSID_MAX_LEN 30
#define PASSWORD_MAX_LEN 30