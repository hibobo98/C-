/*
 WiFiEsp example: WebClient

 This sketch connects to google website using an ESP8266 module to
 perform a simple web search.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/
#include "HX711.h" //HX711로드셀 라이브러리 불러오기
#include <Wire.h>
#include <Adafruit_MLX90614.h>

#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
SoftwareSerial Serial1(6, 7); // RX, TX
float temp1;
float temp2;

#endif

#define calibration_factor -7050.0 // 로드셀 초기값을 설정해줍니다. 이렇게 해주어야 처음에 작동시에 0점을 맞추는 것이라고 생각하시면 됩니다.
#define DOUT1  3 //엠프 데이터 아웃 핀 
#define CLK1  2  //엠프 클락  
HX711 scale1(DOUT1, CLK1); //엠프 핀 선언  

#define DOUT2  5 //엠프 데이터 아웃 핀 
#define CLK2  4  //엠프 클락
HX711 scale2(DOUT2, CLK2); //엠프 핀 선언

char ssid[] = "Sol iphone";            // your network SSID (name)
char pass[] = "abc1234567";// your network password

char DEVICEID1[] = "vD0F07855B614E27";
char DEVICEID2[] = "v2FD29E5D75933AC";
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "api.pushingbox.com";
boolean getIsConnected = false;
String rcvbuf;
// Initialize the Ethernet client object
WiFiEspClient client;

void httpRequest1();
void httpRequest2();
void printWifiStatus();

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  
  //printWifiStatus();

  Serial.println("");
  Serial.println("WiFI connected");
  Serial.println("IP adress: ");
  Serial.println("WiFi.localIP()");

  mlx.begin();
  
  scale1.set_scale();  
  scale1.tare();
  scale2.set_scale();  
  scale2.tare();  
}
  
void loop(){
  temp1 = mlx.readObjectTempC();
  temp2 = mlx.readAmbientTempC();
  
  scale1.set_scale(calibration_factor);
  scale2.set_scale(calibration_factor);

  long weight1 = scale1.get_units()*(0.453592);
  long weight2 = scale2.get_units()*(0.453592);

  Serial.print("weight: ");
  Serial.print(scale1.get_units()*(0.453592), 1);   
  Serial.print(" kg"); //단위
  Serial.println();
  
  Serial.print("weight: ");
  Serial.print(scale2.get_units()*(0.453592), 1);   
  Serial.print(" kg"); //단위
  Serial.println();
  
  Serial.print("Obj: ");
  Serial.print(temp1);
  Serial.println();
  Serial.print("Amb: ");
  Serial.print(temp2);
  Serial.println(); 

    if(weight1<5.0 && weight2>1.0 && weight2<5.0 && temp1>temp2-1.0 && temp1<temp2+1.0)
    {
    httpRequest2();
    }
    else if(weight1<5.0 && weight2>5.0 && temp1>temp2)
    {
    httpRequest1();
    }
    Serial.println();
    delay(1000);
}

void httpRequest1(){
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");

    // send the HTTP PUT request
    client.print(F("GET /pushingbox?devid="));
    client.print(DEVICEID1);
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Host: api.pushingbox.com\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("\r\n\r\n"));

    getIsConnected = true;
  }

  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    getIsConnected = false;
  }
}
void httpRequest2(){
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");

    // send the HTTP PUT request
    client.print(F("GET /pushingbox?devid="));
    client.print(DEVICEID2);
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Host: api.pushingbox.com\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("\r\n\r\n"));

    getIsConnected = true;
  }

  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    getIsConnected = false;
  }
}