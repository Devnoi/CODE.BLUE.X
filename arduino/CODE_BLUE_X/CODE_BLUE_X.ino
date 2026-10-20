#include <WiFiS3.h>
#include <Arduino_LED_Matrix.h>
// ArduinoJson v7 recommended (install from Library Manager)
const char WIFI_SSID[] = "YOUR_WIFI_SSID";
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
const char CLOUD_HOST[] = "your-app.onrender.com"; // No https:// or slash
const char DEVICE_TOKEN[] = "REPLACE_WITH_DEVICE_TOKEN";
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int BUTTON_A = 2; // to GND, optional
const int BUTTON_B = 3; // to GND, optional
const int BUZZER_PIN = 6; // passive piezo to GND, optional
ArduinoLEDMatrix matrix;
WiFiSSLClient client;
unsigned long lastPoll=0,lastReconnect=0,lastWave=0;
bool wasNear=false;
int topic=0;
const uint32_t ICONS[3][3] = {
 {0x0001F800,0x0003FC00,0x0001F800},
 {0x0000F000,0x0001F800,0x0000F000},
 {0x0000FC00,0x0001FE00,0x0000FC00}
};
void indicate(){matrix.loadFrame(ICONS[topic]);}
long distanceCm(){digitalWrite(TRIG_PIN,LOW);delayMicroseconds(2);digitalWrite(TRIG_PIN,HIGH);delayMicroseconds(10);digitalWrite(TRIG_PIN,LOW);unsigned long us=pulseIn(ECHO_PIN,HIGH,24000UL);if(!us)return -1;return long(us/58UL);}
void connectWifi(){if(WiFi.status()==WL_CONNECTED)return;Serial.println("Connecting WiFi...");WiFi.begin(WIFI_SSID,WIFI_PASSWORD);unsigned long start=millis();while(WiFi.status()!=WL_CONNECTED && millis()-start<12000){delay(250);}Serial.println(WiFi.status()==WL_CONNECTED?"WiFi connected":"WiFi failed");}
void sendReading(long cm,bool wave){if(WiFi.status()!=WL_CONNECTED)return;client.stop();if(!client.connect(CLOUD_HOST,443)){Serial.println("TLS connection failed");return;}
 String payload=String("{\"distance\":")+String(cm<0?0:cm)+",\"gesture\":\""+(wave?"wave":"none")+"\"}";
 client.println("POST /api/device HTTP/1.1");client.print("Host: ");client.println(CLOUD_HOST);client.println("Content-Type: application/json");client.print("X-Device-Token: ");client.println(DEVICE_TOKEN);client.print("Content-Length: ");client.println(payload.length());client.println("Connection: close");client.println();client.print(payload);
 unsigned long start=millis();while(client.connected()&&millis()-start<3500){while(client.available()){Serial.write(client.read());start=millis();}delay(1);}client.stop();}
void setup(){Serial.begin(115200);pinMode(TRIG_PIN,OUTPUT);pinMode(ECHO_PIN,INPUT);pinMode(BUTTON_A,INPUT_PULLUP);pinMode(BUTTON_B,INPUT_PULLUP);pinMode(BUZZER_PIN,OUTPUT);matrix.begin();indicate();connectWifi();}
void loop(){unsigned long now=millis();if(WiFi.status()!=WL_CONNECTED&&now-lastReconnect>15000){lastReconnect=now;connectWifi();}
 if(now-lastPoll>=1400){lastPoll=now;long cm=distanceCm();bool near=(cm>=5&&cm<=25);bool wave=near&&!wasNear&&(now-lastWave>1800);wasNear=near;if(wave){lastWave=now;topic=(topic+1)%3;indicate();tone(BUZZER_PIN,880,120);}sendReading(cm,wave);}
 static bool oldA=HIGH,oldB=HIGH;bool a=digitalRead(BUTTON_A),b=digitalRead(BUTTON_B);if(oldA==HIGH&&a==LOW){topic=(topic+1)%3;indicate();tone(BUZZER_PIN,880,100);}if(oldB==HIGH&&b==LOW){tone(BUZZER_PIN,440,140);}oldA=a;oldB=b;delay(25);
}
