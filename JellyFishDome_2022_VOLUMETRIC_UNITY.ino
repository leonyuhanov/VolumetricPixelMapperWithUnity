/*
      Coded for the Wemos D1 Mini
      //Pin out for module
      D6  DATA OUT 1
      D6  DATA OUT 2
*/
#include "SPI.h"
#include "NeoViaSPI.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>


const short int numLeds = 384+1; //dome uses NULL PIXEL to boost voltage
byte tempColour[3] = {0,0,0};
unsigned short int pixelCnt=0, bufferCnt=0;
NeoViaSPI leds = NeoViaSPI(numLeds);

//Network Stuff
//Please enter corect details for wifi details
const char * ssid = "SSID";
const char * password = "WIFIKEY";
WiFiUDP Udp;
const unsigned int udpTXPort = 1000;
const unsigned int packetSize=384*3;
unsigned int rxPacketSize = 0;
byte reciveBuffer[packetSize];
IPAddress serverAddress(192,168,1,111);


void setup()
{
  Serial.begin(115200);
  Serial.println("\r\n\r\n");
  
  //Eable WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
        delay(100);
        Serial.print(".");
  }
  Serial.printf("\r\nMIDI Module ONLINE\r\nCurrent Ip Address:\t");
  Serial.print(WiFi.localIP());
  Serial.print("\r\n");
  Udp.begin(udpTXPort);
  
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setFrequency(3333333);

  for(pixelCnt=0; pixelCnt<numLeds; pixelCnt++)
  {
    leds.setPixel(pixelCnt, tempColour);
  }
  renderLEDs();
}

void loop()
{
  pollUDP();
  yield();
}


void renderLEDs()
{
    leds.encode();
    SPI.writeBytes(leds.neoBits, leds._NeoBitsframeLength);
}

void pollUDP()
{
   rxPacketSize = Udp.parsePacket();
   if(rxPacketSize)
   {
      Udp.read(reciveBuffer, rxPacketSize);
      bufferCnt=0;
      for(pixelCnt=1; pixelCnt<numLeds; pixelCnt++)
      {
        tempColour[0] = reciveBuffer[bufferCnt];
        tempColour[1] = reciveBuffer[bufferCnt+1];
        tempColour[2] = reciveBuffer[bufferCnt+2];
        leds.setPixel(pixelCnt, tempColour);
        bufferCnt+=3;
      }
      renderLEDs();
   }
}
