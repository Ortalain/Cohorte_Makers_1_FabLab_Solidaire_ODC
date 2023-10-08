// ------------------------ Autre Programme Master ------------------------ //

#include "BluetoothSerial.h"
#define _pinBp 35
#define USE_NAME // Comment this to use MAC address instead of a slaveName
const char *pin = "1234"; // Change this to reflect the pin expected by the real slave BT device

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

#ifdef USE_NAME
  String slaveName = "GILET"; // Change this to reflect the real name of your slave BT device
#else
  String MACadd = "AA:BB:CC:11:22:33"; // This only for printing
  uint8_t address[6]  = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33}; // Change this to reflect real MAC address of your slave BT device
#endif


String myName = "BRACELET";
uint8_t LL;
bool connected;
bool go = false;
#define Vibreur 2

void setup() {
  //bool connected;
  Serial.begin(115200);
  pinMode(Vibreur, OUTPUT);
  pinMode(_pinBp, INPUT);

  SerialBT.begin(myName, true);
  Serial.printf("The device \"%s\" started in master mode, make sure slave BT device is on!\n", myName.c_str());

  #ifndef USE_NAME
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif

  // connect(address) is fast (up to 10 secs max), connect(slaveName) is slow (up to 30 secs max) as it needs
  // to resolve slaveName to address first, but it allows to connect to different devices with the same name.
  // Set CoreDebugLevel to Info to view devices Bluetooth address and device names
  #ifdef USE_NAME
    connected = SerialBT.connect(slaveName);
    Serial.printf("Connecting to slave BT device named \"%s\"\n", slaveName.c_str());
  #else
    connected = SerialBT.connect(address);
    Serial.print("Connecting to slave BT device with MAC "); Serial.println(MACadd);
  #endif

  if(connected) {
    Serial.println("Connected Successfully!");
  } else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
    }
  }
  // Disconnect() may take up to 10 secs max
  if (SerialBT.disconnect()) {
    Serial.println("Disconnected Successfully!");
  }
  // This would reconnect to the slaveName(will use address, if resolved) or address used with connect(slaveName/address).
  SerialBT.connect();
  if(connected) {
    Serial.println("Reconnected Successfully!");
  } else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to reconnect. Make sure remote device is available and in range, then restart app.");
    }
  }
}










void loop() {
  unsigned long deb = millis();
  while(digitalRead(_pinBp)){ // presser le bouton pendant 2 seconde pour sortir du mode veille
    if(millis() - deb > 2000){go = true;}
  }
  Serial.println(" ... ");
  while(go){
    connected = SerialBT.connect(slaveName);
    if(connected) 
    {
      Serial.println("Connected Successfully!");
      SerialBT.write(1);
      delay(10);
    } 
    else 
    {
      while(!SerialBT.connected(500) && !SerialBT.connect(slaveName)) {
        Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
        digitalWrite(Vibreur, LOW);
        delay(5000);
        digitalWrite(Vibreur, HIGH);
      }
      digitalWrite(Vibreur, LOW);
    }
    
    
    deb = millis();
    while(digitalRead(_pinBp)){ // presser le bouton pendant 2 seconde pour mettre en veille
      if(millis() - deb > 2000){go = false;}
    }
  }
}