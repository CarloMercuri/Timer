#pragma once

struct WifiConfig {
  char* serialNumber;
  char* ssid;
  char* password;
};

#define SSID_MAX_LEN 30
#define PASSWORD_MAX_LEN 30