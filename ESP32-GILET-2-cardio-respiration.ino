#include "DetectPulse.h"
#define sortie 36

DetectPulse detecteurP(4000, sortie, 280);


//-------------------------------------------------------------------------------------------------------------------------------
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "DetectBreath.h"

DetectBreath detecteurB(10,3,200,30000); // nbMin = 10;  ecartMinIndex = 3; delaiEntreEnregistrement = 200; collectTime = 30000;


//-------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>


#define WIFI_SSID "WIFI ODC"
#define WIFI_PASSWORD "Digital1"
#define API_KEY "AIzaSyAGM_IqJ9dN6O4Ybuig5ZpJLzFy--F_y7Y"
#define DATABASE_URL "https://fir-oc-b0d1e-default-rtdb.europe-west1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define USER_EMAIL "nombresalif225@gmail.com"
#define USER_PASSWORD "junior225"


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;



char* _rootPath = "distance/aaaa/";
unsigned long deb;
String _path;

/////// -------------- PATHs
char* _pathPulse = "frequenceCardiaque";
char* _pathBreath = "frequenceRespiratoire";
////// --------------- VALUES
int _valuePulse = 10; 
int _valueBreath = 12; 
int _valueInt = 12;


void setup()
{
  
  Serial.begin(115200);

  //---------------------------------------------------------------------------------------------------------------------------------------
  if (!detecteurB.begin()) {Serial.println("Failed to find MPU6050 chip");}
  delay(100);
  pinMode(sortie, INPUT);


  //----------------------------------------------------------------------------------------------------------------------------------------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("."); delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, USER_EMAIL, USER_PASSWORD)){
      Serial.println("signUp OK");
      signupOK = true;
      }
      else{
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
        }
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
}

void loop()
{
    
    
    _valueBreath = detecteurB.work();
    Serial.println(_valueBreath);
    _valuePulse = detecteurP.work();
    Serial.println(_valuePulse);

    if(Firebase.ready() && true ){
      for(int i=0; i<2; i++){ 
        switch(i){
          case 0: _valueInt = _valueBreath;
          _path = _pathBreath;
            break;
          case 1: _valueInt = _valuePulse;
          _path = _pathPulse;
            break;
        }
        
        if(Firebase.RTDB.setInt(&fbdo, _path, _valueInt)){
          Serial.println(" SUCCESS DATA ");
          }
        else{
            Serial.println("FAILED: "+ fbdo.errorReason());     
          };
        deb = millis();
        while(millis()-deb < 100){;};
      }
  }
    deb = millis();
    while(millis()-deb < 5000){;};
  Serial.println(" ... ");

}