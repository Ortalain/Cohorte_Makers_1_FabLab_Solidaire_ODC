#include <Arduino.h>
#include <HardwareSerial.h>
const int batteryPin = 35;  // Broche analogique utilisée pour mesurer la batterie
const int nb=10 ;
int pred = 100;
const float maxVoltage = 3.3;     // Tension maximale de la batterie en volts
const float minVoltage = 0;     // Tension minimale de la batterie en volts
const float maxBatteryPercentage = 100.0;  // Pourcentage correspondant à la tension maximale
const float minBatteryPercentage = 0.0;    // Pourcentage correspondant à la tension minimale

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#include <TinyGPS++.h>

HardwareSerial gpsSerial(2);
TinyGPSPlus gps;
unsigned long debut;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#define WIFI_SSID "WIFI ODC"
#define WIFI_PASSWORD "Digital1"
#define API_KEY "AIzaSyAmPolMBRDS8hAQYAUq_FJ9mZdWo8vcjJU" //"AIzaSyAGM_IqJ9dN6O4Ybuig5ZpJLzFy--F_y7Y"
#define DATABASE_URL "https://poubelle-connecte-odc-default-rtdb.firebaseio.com/" //"https://fir-oc-b0d1e-default-rtdb.europe-west1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
//#define USER_EMAIL "nombresalif225@gmail.com"
//#define USER_PASSWORD "junior225"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
bool BD = false;
char* _rootPath = "distance/aaaa/";
unsigned long deb;
String _path;
String result;

/////// -------------- PATHs
char* _pathBat = "nivauBaterie";
char* _pathTemp = "temperatureRealtime";
char* _pathGps = "GPS";

////// --------------- VALUES
float _valueGpsLo = 30.0;
float _valueGpsLa = 30.0; 
float _valueFloat = 30.0;
int _valueTemp = 30;
int _pinBat = 32;
String _valueBaterie = "100%";

void setup()
{

  Serial.begin(115200);
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  
  gpsSerial.begin(9600, SERIAL_8N1,16, 17);
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
  //auth.user.email = USER_EMAIL;
  //auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  //if(Firebase.signUp(&config, &auth, USER_EMAIL, USER_PASSWORD)){
  if(Firebase.signUp(&config, &auth, "", "")){
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
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
  };
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}

void loop()
{

  debut = millis();
  while ( gpsSerial.available() > 0 && millis()-debut<300000) { // pendant 5 min
    //if(gpsSerial.available() > 0){
      if (gps.encode(gpsSerial.read())) {
        if (gps.location.isValid()) {
          Serial.print("La: ");
          _valueGpsLa = gps.location.lat();
          Serial.print(_valueGpsLa);
          
          Serial.print(", Lo: ");
          _valueGpsLo = gps.location.lng();
          Serial.println(_valueGpsLo);
        } else {
          Serial.println("GPS signal non valide");
        }
      }
    //}
  }

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    /////// ----------------------------- ENVOI DES DONNEES --------------------------------------------------------------------------------------------------------------------


    /////////////////////////// NIVEAU BAT -------------------------------------------------------------------------------------------------------------------------------------
    int rawValue = analogRead(batteryPin);  // Lecture de la valeur brute depuis la broche A0
    float voltage = (rawValue * maxVoltage) / 4095.0;
    float somme=0;
    for(int i=0;i<nb;i++){
      somme+=voltage;
      delay(100); 
    }
    float Vmoyenne=(float)somme/nb;
    float batteryPercentage = int(Vmoyenne*100)/3.3;
    if(pred > batteryPercentage){
        pred = batteryPercentage;
    }
    _valueBaterie = String(pred)+String("%");
    Serial.println(_valueBaterie);
    _path = String(_rootPath)+String(_pathBat);
    if(Firebase.RTDB.setString(&fbdo, _path, _valueBaterie)){ // niveau de batterie
      Serial.println(" SUCCESS BATERIE LEVEL ");
      }0
    else{
        Serial.println("FAILED: "+ fbdo.errorReason());
      } 
    deb = millis();
    while(millis()-deb < 100){;};


    /////////////////////////// TEMPERATURE ------------------------------------------------------------------------------------------------------------------------------------
    _path = String(_rootPath)+String(_pathTemp);
    _valueTemp = int(mlx.readObjectTempC());
    Serial.println(_valueTemp);
    if(Firebase.RTDB.setInt(&fbdo, _path, _valueTemp)){
      Serial.println(" SUCCESS TEMPERATURE ");
      }
    else{
        Serial.println("FAILED: "+ fbdo.errorReason());     
      };
    deb = millis();
    while(millis()-deb < 100){;};
  


    /////////////////////////// GPS LATITUDE & GPS LONGITUDE --------------------------------------------------------------------------------------------------------------------
    for(int i=0; i<2; i++){
      switch(i){
        case 0: _valueFloat = _valueGpsLo; _path = String(_rootPath)+String(_pathGps)+String("Longitude");
          break;
        case 1: _valueFloat = _valueGpsLa; _path = String(_rootPath)+String(_pathGps)+String("Latitude");
          break;
      }
      if(Firebase.RTDB.setFloat(&fbdo, _path, _valueFloat)){  // temperature, gpsLo, gpsLa
        Serial.println(" SUCCESS FLOAT ");
        }
      else{
          Serial.println(" FAILED GPS ");     
        };
      deb = millis();
      while(millis()-deb < 100){;};
    }

    deb = millis();
    while(millis()-deb < 5000){;}; // attente

  Serial.println(" ... ");
}