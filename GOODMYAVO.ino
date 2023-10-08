//-------------------------------------------------------------------------------------
#include "BluetoothSerial.h"
const char *pin = "1234"; // Change this to more secure PIN.
String device_name = "GILET";
char* contact_1 = "+2250797349279";
char* contact_2 = "+2250748966409";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#include "App.h"
BluetoothSerial SerialBT;
unsigned long debut = 0, debutApp = 0;



void setup() {
  beginModule();
  //Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name GILET is started.\nNow you can pair it with Bluetooth!\n");
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
  //------------------------------------------------------------------------------------
  digitalWrite(LED_GPIO, LED_ON);
  while(!SerialBT.available()){
      Serial.println("Waiting for Master... ");
    };
}

void loop() {
  while(millis() - debut < 180000){ // 3 min
    while(SerialBT.available()){
      Serial.write(SerialBT.read());
      Serial.println();
      Serial.println("1");
      debut = millis();
    };
    Serial.println(" ... ");
  }
  //----------------------------------------------------------------------------
  Serial.println(" DECONNECTED ");

  call(contact_1);
  sms(contact_1, "L\'enfant est trop loin de l\'accompagnant(e)");
  delay(60000);
  // ----------------------------------------------------------------------

  call(contact_2);
  sms(contact_2, "L\'enfant est trop loin de l\'accompagnant(e)");
  debut = millis();

}

