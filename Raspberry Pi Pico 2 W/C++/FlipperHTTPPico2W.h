/* FlipperHTTPPico.h for flipper-http-pico.ino (for the Raspberry Pi Pico 2 W)
Author: JBlanked
Github: https://github.com/jblanked/FlipperHTTP
Info: This library is a wrapper around the HTTPClient library and is used to communicate with the FlipperZero over serial.
Board Manager: Raspberry Pi Pico W
Created: 2024-12-07
Updated: 2024-12-07

Change Log:
- 2024-12-07: Initial commit
*/
#ifndef FLIPPER_HTTP_PICO_2W
#define FLIPPER_HTTP_PICO_2W

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <LittleFS.h> // replacement for SPIFFS

// Define UART parameters
#define BAUD_RATE 115200
#define PICO_LED LED_BUILTIN
#define ON HIGH
#define OFF LOW

class FlipperHTTP
{
public:
  // Constructor
  FlipperHTTP()
  {
  }

  void clearSerialBuffer();                                                                                                  // Clear serial buffer to avoid any residual data
  bool connectToWifi();                                                                                                      // Connect to Wifi using the loaded SSID and Password
  String delete_request(String url, String payload);                                                                         // Delete request
  String delete_request(String url, String payload, const char *headerKeys[], const char *headerValues[], int headerSize);   // Delete request with headers
  String get(String url);                                                                                                    // Get request
  String get(String url, const char *headerKeys[], const char *headerValues[], int headerSize);                              // Get request with headers
  bool get_bytes_to_file(String url, const char *headerKeys[], const char *headerValues[], int headerSize);                  // Get request to process bytes
  String getIPAddress() { return WiFi.localIP().toString(); }                                                                // get IP addresss
  bool isConnectedToWifi() { return WiFi.status() == WL_CONNECTED; }                                                         // Check if the Pico is connected to Wifi
  void ledAction(int timeout = 250);                                                                                         // Function to flash the LED
  void ledStart();                                                                                                           // Display LED sequence when the Pico is first connected to the Flipper
  void ledStatus();                                                                                                          // Starting LED (Green only)
  void ledOff() { digitalWrite(PICO_LED, OFF); }                                                                             // Turn off LED
  bool loadWifiSettings();                                                                                                   // Load Wifi settings from SPIFFS
  String post(String url, String payload);                                                                                   // Post request
  String post(String url, String payload, const char *headerKeys[], const char *headerValues[], int headerSize);             // Post request with headers
  bool post_bytes_to_file(String url, String payload, const char *headerKeys[], const char *headerValues[], int headerSize); // Post request to process bytes
  String put(String url, String payload);                                                                                    // Put request
  String put(String url, String payload, const char *headerKeys[], const char *headerValues[], int headerSize);              // Put request with headers
  bool saveWifiSettings(String data);                                                                                        // Save Wifi settings to LittleFS
  String scanWifiNetworks();                                                                                                 // Scan for Wifi networks (returns a string of all wifi networks)
  void setup();                                                                                                              // Arduino setup function
  String readSerialLine();                                                                                                   // Read serial data until newline character
  bool readSerialSettings(String receivedData, bool connectAfterSave);                                                       // Read the serial data and save the settings

  void loop(); // Main loop for flipper-http-pico.ino that handles all of the commands
private:
  const char *settingsFilePath = "/flipper-http.json"; // Path to the settings file in the SPIFFS file system
  char loadedSSID[64] = {0};                           // Variable to store SSID
  char loadedPassword[64] = {0};                       // Variable to store password
  bool useLED = true;                                  // Variable to control LED usage
  WiFiClientSecure client;                             // WiFiClientSecure object for secure connections
  SerialPIO *SerialPico;                               // Serial object for the Pico

  // Root CA from letsencrypt
  // get it here: https://letsencrypt.org/certificates/
  const char *root_ca =
      "-----BEGIN CERTIFICATE-----\n"
      "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\n"
      "-----END CERTIFICATE-----\n";
};

#endif