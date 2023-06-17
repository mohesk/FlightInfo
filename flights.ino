#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <LiquidCrystal_I2C.h>

const char* ssid       = "";
const char* password   = "";
const char* test_root_ca= "";
"-----END CERTIFICATE-----\n";
const char* RT_flight_data_url = "https://data-cloud.flightradar24.com/zones/fcgi/feed.js?bounds=";
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
void setup() {
  lcd.init();
  lcd.noBacklight();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }
}
void loop() {
    if(WiFi.status()== WL_CONNECTED){
     String AllFlightsReadings = GetFlight(RT_flight_data_url);
      JSONVar allFlights = JSON.parse(AllFlightsReadings);
      JSONVar keys = allFlights.keys();
      Serial.println(keys);
        for (int i = 0; i < keys.length(); i++) {
          String flightId=JSON.stringify(keys[i]);          
          if (isDigit(flightId[1])){
            JSONVar value = allFlights[keys[i]];
            String ORG_Air_Name = GetAirport(JSON.stringify(value[11]));    
            ORG_Air_Name.replace("""","");
            ORG_Air_Name = "From: " + ORG_Air_Name;
            ORG_Air_Name.replace("Airport","");
            ORG_Air_Name.replace("International","Int.");
            char TempORG_Air_Name[20];
            ORG_Air_Name.toCharArray(TempORG_Air_Name, 21);
            String DES_Air_Name = GetAirport(JSON.stringify(value[12]));
            DES_Air_Name.replace("""","");
            DES_Air_Name = "To: " + DES_Air_Name;
            DES_Air_Name.replace("Airport","");
            DES_Air_Name.replace("International","Int.");
            char TempDES_Air_Name[20];
            DES_Air_Name.toCharArray(TempDES_Air_Name, 21);
            lcd.clear();
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print(TempORG_Air_Name);
            lcd.setCursor(0, 1);
            lcd.print(TempDES_Air_Name);
            String Speed = JSON.stringify(value[5]);
            Speed = Speed + " kts";
            String Altitude = JSON.stringify(value[4]);
            Altitude = Altitude + " feet / " + Speed;
            lcd.setCursor(0, 2);
            lcd.print(Altitude);
            String AirCraft = JSON.stringify(value[8]);
            AirCraft = "AirCraft: " + AirCraft ;
            lcd.setCursor(0, 3);
            lcd.print(AirCraft);
            delay(1000);
          }
        }
    }
  }
String GetFlight(const char* serverName) {
  WiFiClientSecure client;
  HTTPClient http;
  client.setCACert(test_root_ca);
  http.begin(client,serverName);
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  if (httpResponseCode>0) 
    payload = http.getString();
  http.end();
  return payload;
}
String GetAirport(String AirportCode){
  WiFiClientSecure client;
  HTTPClient http;
  client.setCACert(test_root_ca);
  AirportCode.remove(0,1);
  AirportCode.remove(3,1);
  String Server = "https://www.flightradar24.com/airports/traffic-stats/?airport="+AirportCode;
  http.begin(client,Server);
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  if (httpResponseCode>0) 
    payload = http.getString();
  http.end();
  JSONVar OriginAirport = JSON.parse(payload);
  JSONVar OriginAirportName = OriginAirport["details"];
  String ORG_Air_Name = OriginAirportName["name"];
  return ORG_Air_Name;
}