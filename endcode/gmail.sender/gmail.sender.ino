#include <ESP8266WiFi.h>
#include "Gsender.h"
#include "timer.h"

#pragma region Globals
const char* ssid = "Arduino";                           // WIFI network name
const char* password = "@rdu1n0s";                       // WIFI network password
uint8_t connection_state = 0;                    // Connected to WIFI or not
uint16_t reconnect_interval = 10000;             // If not connected wait time to try again
#pragma endregion Globals

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
  static uint16_t attempt = 0;
  Serial.print("Connecting to ");
  if (nSSID) {
    WiFi.begin(nSSID, nPassword);
    Serial.println(nSSID);
  } else {
    WiFi.begin(ssid, password);
    Serial.println(ssid);
  }

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 50)
  {
    delay(200);
    Serial.print(".");
  }
  ++attempt;
  Serial.println("");
  if (i == 51) {
    Serial.print("Connection: TIMEOUT on attempt: ");
    Serial.println(attempt);
    if (attempt % 2 == 0)
      Serial.println("Check if access point available or SSID and Password\r\n");
    return false;
  }
  Serial.println("Connection: ESTABLISHED");
  Serial.print("Got IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

void Awaits()
{
  uint32_t ts = millis();
  while (!connection_state)
  {
    delay(50);
    if (millis() > (ts + reconnect_interval) && !connection_state) {
      connection_state = WiFiConnect();
      ts = millis();
    }
  }
}

void sender() {
  connection_state = WiFiConnect();
  if (!connection_state) // if not connected to WIFI
    Awaits();          // constantly trying to connect
    
  Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
  if (gsender->Subject(subject)->Send("marceloloch.newaa@gmail.com", assunto)) {
    Serial.println("Message send.");
  } else {
    Serial.print("Error sending message: ");
    Serial.println(gsender->getError());
  }

}
void setup()
{
  Serial.begin(115200);
  config_setup ();
}

void loop() {
  //verificacao no loop
  valor_AD = analogRead(A0);
  soma = soma + valor_AD;
  i++;

  if ( i == 100) {
    media = soma / i;
    Serial.println(media);
    i = 0;
    soma = 0;
  }else if (media > 50 && (state == 0 || state == 1)) {
     reset_timer1();
     state = 3;
  } else if (_timeout2 && (state == 0 || state == 2)) {
    state = 1;
    digitalWrite(D1, HIGH);
    assunto = "A rádio está fora do ár ou muda.";
    subject = "Radio - OFF";
    sender();
    _timeout1 = false;
  }else if (media > 50 && _timeout1 && (state == 0 || state == 3)) {
    state = 2;
    digitalWrite(D1, HIGH);
    subject = "Radio - ON";
    assunto = "A rádio está ativa";
    sender();
    _timeout2 = false;
  }

  if (i == 1 && media > 45) {
    reset_timer2();
    digitalWrite(D1, LOW);
    yield();
  }
}  
