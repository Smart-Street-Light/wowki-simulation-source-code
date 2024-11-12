#include "WiFi.h"
#include "ArduinoJson.h"
#define AO_PIN 36
WiFiClient client;
String thingSpeakAddress = "api.thingspeak.com";
String request_string;
int prev = -1;
void setup()
{
  Serial.begin(115200);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  digitalWrite(26, HIGH);
  WiFi.disconnect();
  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

bool isemergency()
{

  if (client.connect("api.thingspeak.com", 80))
  {
    Serial.println(String("GET ") + "" + " HTTP/1.1\r\n" +
                   "Host: " + thingSpeakAddress + "\r\n" +
                   "Connection: close\r\n\r\n");

    client.print(String("GET ") + "" + " HTTP/1.1\r\n" +
                 "Host: " + thingSpeakAddress + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return false;
      }
    }
    while (client.available())
    {
      String line = client.readStringUntil('\r');
      int n = line.length();
      // char *json = line;

      JsonDocument doc;
      deserializeJson(doc, line);
      const char *x = doc["feeds"][0]["field1"];
      Serial.print(x);
      if (x[0] == '1')
        return true;
    }
  }
  return false;
}
void loop()
{
  delay(2000);

  int lightValue = analogRead(AO_PIN);

  //  if(prev!=lightValue) {
  //   DHT_thingspeak(lightValue);
  //   prev=lightValue;
  //  }
  // else{
  //   Serial.println("nochange");
  // }

  bool i = isemergency();
  if (i)
    digitalWrite(25, HIGH);
  else
    digitalWrite(25, LOW);
  Serial.print(i);
}
void DHT_thingspeak(float temp)
{
  if (client.connect("api.thingspeak.com", 80))
  {
    request_string = "/update?";
    request_string += "key=";
    request_string += "I1GFDDPR9UNSVUCO";
    request_string += "&";
    request_string += "field1";
    request_string += "=";
    request_string += temp;
    Serial.println(String("GET ") + request_string + " HTTP/1.1\r\n" +
                   "Host: " + thingSpeakAddress + "\r\n" +
                   "Connection: close\r\n\r\n");

    client.print(String("GET ") + request_string + " HTTP/1.1\r\n" +
                 "Host: " + thingSpeakAddress + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    while (client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    Serial.println();
    Serial.println("closing connection");
  }
}