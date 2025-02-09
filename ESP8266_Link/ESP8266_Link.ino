#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

// CON=NoAccess,qazwsxedc123,
// WEB=1,?$

const char link0[] = "http://esinebd.com/projects/loadshedding/update_machine.php";
const char link1[] = "http://esinebd.com/projects/loadshedding/stat_machine.php";

ESP8266WiFiMulti wifi;
WiFiClient client;
HTTPClient http;
WiFiServer server(8080);

int i;
String ssid, pass, link;
byte MODE = 0;
bool serverMode;
String load = "000000";

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  WiFi.begin("NoAccess", "qazwsxedc123");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  checkCMD();

  if (MODE == 1) {
    client = server.available();
    if (client) {
      while (client.connected()) {
        if (client.available()) Serial.println(client.readStringUntil('\n'));
        if (Serial.available()) client.println(Serial.readStringUntil('\n'));
      }
    }
  } else if (MODE == 2) {
    if (client) {
      while (client.connected()) {
        if (client.available()) Serial.println(client.readStringUntil('\n'));
        if (Serial.available()) client.println(Serial.readStringUntil('\n'));
      }
    }
  } else if (MODE == 3 || MODE == 4) {
    if (http.begin(client, link.c_str())) {
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String cmd = http.getString();
        if (MODE == 4) {
          if (cmd.indexOf("sc=1") != -1) serverMode = 1;
          else if (cmd.indexOf("sc=0") != -1) serverMode = 0;

          if (serverMode == 1) {
            if (cmd.indexOf("l1=1") != -1) load[0] = '1';
            else if (cmd.indexOf("l1=0") != -1) load[0] = '0';

            if (cmd.indexOf("l2=1") != -1) load[1] = '1';
            else if (cmd.indexOf("l2=0") != -1) load[1] = '0';

            if (cmd.indexOf("l3=1") != -1) load[2] = '1';
            else if (cmd.indexOf("l3=0") != -1) load[2] = '0';

            if (cmd.indexOf("l4=1") != -1) load[3] = '1';
            else if (cmd.indexOf("l4=0") != -1) load[3] = '0';

            if (cmd.indexOf("l5=1") != -1) load[4] = '1';
            else if (cmd.indexOf("l5=0") != -1) load[4] = '0';

            if (cmd.indexOf("l6=1") != -1) load[5] = '1';
            else if (cmd.indexOf("l6=0") != -1) load[5] = '0';

            Serial.println("l=" + load);
          }
          else Serial.println(cmd);
        }
      }
    }
    MODE = 0;
  }
}

void checkCMD() {
  if (Serial.available()) {
    String rx = Serial.readStringUntil('\n');

    if (rx.indexOf("AT") != -1) Serial.println("OK");
    else if (rx.indexOf("CON=") != -1) {
      rx.remove(0, 4);
      i = rx.indexOf(",");
      ssid = rx.substring(0, i);
      rx.remove(0, i + 1);
      i = rx.indexOf(",");
      pass = rx.substring(0, i);
      wifi.addAP(ssid.c_str(), pass.c_str());
      if (wifi.run() == WL_CONNECTED) Serial.println("OK");
      else Serial.println("ERROR!");
      MODE = 0;
    } else if (rx.indexOf("SERVER") != -1) {
      server.begin();
      Serial.print("IP:");
      Serial.println(WiFi.localIP());
      Serial.println("OK");
      MODE = 1;
    } else if (rx.indexOf("CLIENT=") != -1) {
      rx.remove(0, 7);
      Serial.println(rx);
      if (!client.connect(rx.c_str(), 8080)) Serial.println("ERROR");
      else Serial.println("CONNECTED");
      Serial.println("OK");
      MODE = 2;
    } else if (rx.indexOf("WEB=0,") != -1) {
      rx.remove(0, 6);
      link = link0 + rx;
      i = link.indexOf("$");
      link.remove(i);
      Serial.println("OK");
      MODE = 3;
    } else if (rx.indexOf("WEB=1,") != -1) {
      rx.remove(0, 6);
      link = link1 + rx;
      i = link.indexOf("$");
      link.remove(i);
      Serial.println("OK");
      MODE = 4;
    }
  }
}
