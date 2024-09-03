#pragma once

struct WifiConfig {
  String ssid;
  String password;
  String serialNumber;

  WifiConfig(String serialNumber, String ssid, String password) : serialNumber(serialNumber), ssid(ssid), password(password) {} 
};

#define SSID_MAX_LEN 30
#define PASSWORD_MAX_LEN 30