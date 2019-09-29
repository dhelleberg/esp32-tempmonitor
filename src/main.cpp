#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/*Put your SSID & Password*/
const char* ssid = "#";  // Enter SSID here
const char* password = "#";  //Enter Password here

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */


// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void send_tmp_data(float temperature) {
  //first we need to connect to InfluxDB server   
  HTTPClient http;    //Declare object of class HTTPClient

  http.begin("http://192.168.178.21:8086/write?db=triops&precision=s");

  http.addHeader("Content-Type", "--data-binary");  //Specify content-type header, I have figured out that it   can be left out

  String postData = "water_temp value="+String(temperature,2);

  int httpCode = http.POST(postData);

  Serial.println(httpCode);

  http.end();  //Close connection

}

float readTemperature() {
  sensors.requestTemperatures(); 
  return sensors.getTempCByIndex(0);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  Serial.setDebugOutput(true);
  WiFi.disconnect();
  delay(1000);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.enableSTA(true);
  delay(1000);
  
  WiFi.setSleep(false);
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
  

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");

  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  Serial.println("init sensor...");
  sensors.begin();
  Serial.println("done");

  float temp = readTemperature();
  send_tmp_data(temp);
  Serial.println("sending data: "+String(temp));
  delay(1000);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
}

void loop() {
 
}

