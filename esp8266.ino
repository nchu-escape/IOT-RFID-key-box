/*
pin   arduino
GND   GND
VCC   VCC
*/

#include <ESP8266WiFi.h>
#include <aREST.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Create aREST instance
aREST rest = aREST();
// WiFi parameters
const char *ssid = "wifi_ssid";
const char *password = "wifi_pass";
// The port to listen for incoming TCP connections
#define LISTEN_PORT 80
// Create an instance of the server
WiFiServer server(LISTEN_PORT);
// Variables to be exposed to the API
int lock=0;
long unsigned int T=0;

const int wifi_signal_send =  12; 
const int wifi_signal_receive =  14;

void setup(void)
{
  pinMode(14, INPUT);
  pinMode(12, OUTPUT);
  Serial.begin(115200);

  rest.variable("lock", &lock);
  rest.set_id("60");         //need change
  rest.set_name("RFID"); //need change
  rest.function("test", test);
  rest.function("set_reset", set_reset);
  WiFi.enableSTA(true);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Start the server
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.println(WiFi.localIP());
  T=millis();
}

void loop()
{
  // Handle REST calls
  WiFiClient client = server.available();
  /////
  if(digitalRead(14)==HIGH){
    if(millis()-T>5000 && lock==0){
      if(wifi_get())
        lock=1;
    }
  }else{
    digitalWrite(12, LOW);
    T=millis();
  }
  /////
  if (!client)
    return;
  while (!client.available())
    delay(1);
  rest.handle(client);
}

int wifi_get(){
  WiFiClient client;
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  if (http.begin(client, "http://<ip>:<port>/getRFID")) {  // HTTP
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    return 1;
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
    return 0;
  }
}

int test(String command) {
  wifi_get();
  return 1;
}

int set_reset(String command){
  digitalWrite(12, HIGH);
  return 1;
}
