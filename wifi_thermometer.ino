#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#define DHTPIN D6
#define DHTTYPE DHT22
 
DHT dht(DHTPIN, DHTTYPE);

// wifi setup every row : SSID, Password
const char* wifi_ssid[][2] = { 
  {"SSID","password"}, 
};
const int wifi_ssid_max = 1;

const String privateAjaxUrl = "";   // link to your json processor website
const String stationID = "D1_dht22_001";

int stationTemp = 0;
int stationHeatIndex = 0;
int stationHumidity = 0;
unsigned long stationMillis = 0;  // timestamp for json data
int sleepSeconds = 600;

/**
 * Set wifi connection
 */
void setWifiConnection() {
  byte available_networks = 0;
  byte foundSsid = false;
  
  do {    
    Serial.println("Searching routers");
    do {
      available_networks = WiFi.scanNetworks();      
      if (available_networks == 0) { 
        Serial.print("."); 
        delay(1000); 
      }
    } while (available_networks == 0);
    
    if (available_networks>0) {
      foundSsid = false;
      Serial.println("Wifi connections available");
      for (int network = 0; network < available_networks; network++) {
        for (int f = 0; f<wifi_ssid_max; f++) {
          if (WiFi.SSID(network) == wifi_ssid[f][0]) {
            Serial.print("Found ");  
            Serial.println(wifi_ssid[f][0]);  
            WiFi.begin(wifi_ssid[f][0], wifi_ssid[f][1]);        
            network = available_networks;
            foundSsid = true;
          }
        }
      }
      byte wifiCounter = 30;
      while ((foundSsid == true) && (WiFi.status() != WL_CONNECTED) && ( wifiCounter > 0)) {
        wifiCounter --;
        delay(500);
        Serial.print(".");
      }
    }  
  } while (WiFi.status() != WL_CONNECTED);  
}

/*
 * Url Encode for sending post/get to webserver
 * @param string original string
 * @return string encoded string
*/
String urlencode(String str) {
  String encodedString="";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i =0; i < str.length(); i++){
    c=str.charAt(i);
    if (c == ' '){
      encodedString+= '+';
    } else if (isalnum(c)){
      encodedString+=c;
    } else{
      code1=(c & 0xf)+'0';
      if ((c & 0xf) >9){
          code1=(c & 0xf) - 10 + 'A';
      }
      c=(c>>4)&0xf;
      code0=c+'0';
      if (c > 9){
          code0=c - 10 + 'A';
      }
      code2='\0';
      encodedString+='%';
      encodedString+=code0;
      encodedString+=code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;   
}

/*
 * Weather information in json format
 */
String jsonOutput() {
  String j = "";
  j+="{\n";

  j += "\"station_id\":\"";
  j += stationID;
  j+="\",\n"; 
  j += "\"millis\":";
  j += stationMillis;
  j+=",\n"; 
  j += "\"humidity\":";
  j += stationHumidity;
  j+=",\n";
  j += "\"temperature\":";
  j += stationTemp;
  j+=",\n";  
  j += "\"heatIndex\":";
  j += stationHeatIndex;
  j+="\n";
  
  j+="}\n";
  return j;
}

/*
 * Send json data with POST method to my webserver
*/ 
void sendRawAjax() {
  HTTPClient http;
  http.begin( privateAjaxUrl );
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("json=" + urlencode(jsonOutput()));
  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();  
  Serial.println(jsonOutput());
}

/**
 * DHT data read and set global variable
 */
void readDHT() {
  String message = "";
  int h = round(dht.readHumidity());
  int t = round(dht.readTemperature());
  

  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    int hic = round(dht.computeHeatIndex(t, h, false));     
    if ((t>-50) && (t<60)) {
      stationTemp = t;
    }
    if ((hic>-50) && (hic<60)) {
      stationHeatIndex = hic;
    }
    if ((h>0) && (h<150)) {
      stationHumidity = h;   
    }
  }
}

/**
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>   SYSTEM SETUP
 */
void setup() {
  Serial.begin(74880); 
  setWifiConnection();
   
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway IP address: ");
  Serial.println(WiFi.gatewayIP());
  dht.begin();
}

/**
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>   SYSTEM LOOP
 */
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setWifiConnection();
  }
  Serial.println("Waiting 10 seconds for the sensor.");
  delay(10000);
  readDHT();
  stationMillis = millis();
  sendRawAjax();
 
  Serial.print("Humidity: "); 
  Serial.print(stationHumidity);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(stationTemp);
  Serial.print(" *C ");
  Serial.print("Heat index: "); 
  Serial.print(stationHeatIndex);
  Serial.println(" *C ");
  
  Serial.print("Going to sleep till ");
  Serial.print(sleepSeconds);
  Serial.println(" seconds. ");
  
  ESP.deepSleep(sleepSeconds * 1000000);
}
