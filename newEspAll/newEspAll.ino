#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define soil  A0
#define setPoint  1
#define relay 5

#define WIFI_SSID     "golek-gratisan-ae"
#define WIFI_PASSWORD "gakisokkonek"

WiFiClient client;
String serve = "AAAAGmKd_YU:APA91bHKX-duM1ZzC97Epy53qY20POVIslbZ6pEsPJlgfe4L1NbyF1AJiHokCBQi-w5LkIh36uwixJpwZUgy2Vce-x2NuEEEg71-zXUkl9oluNqCn97FXknODtRg9ZAF4Z7GcLphufdD";
String reg = "dqAxIqI1tJs:APA91bE9uhlIhYHyeOsXL_oPUcSu_qjlHMTKFXC1fuG_ixw-zWaBiopPFNiyms83x4okHgaW4EOgpinAfPRvg2a6BVaqjLz_q9cpiwcmieb1kv-CE9BqNxQOcsmnM28n8foDbvnp21eh";

boolean afterOn = false;
boolean afterOff = false;


void setup() {
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  Serial.setDebugOutput(true);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
 
}


HTTPClient http;
void doit(String paytitle, String pay) {
  //more info @ https://firebase.google.com/docs/cloud-messaging/http-server-ref

  String data = "{";
  data = data + "\"to\": \"" + reg + "\",";
  data = data + "\"notification\": {";
  data = data + "\"body\": \"" + pay + "\",";
  data = data + "\"title\" : \"" + paytitle + "\" ";
  data = data + "} }";

  http.begin("http://fcm.googleapis.com/fcm/send");
  http.addHeader("Authorization", "key=" + serve);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Host", "fcm.googleapis.com");
  http.addHeader("Content-Length", String(pay.length()));
  http.POST(data);
  http.writeToStream(&Serial);
  http.end();
  Serial.println();

}

void loop() {
  //doit("asdf", "silver"); 
 
  //delay(13000);

  int rawSoil = 1024 - analogRead(soil);
  int mappingSoil = map(rawSoil, 30, 1024, 0, 100);
  if(mappingSoil > 100) {mappingSoil = 100;}
  if(mappingSoil < 0) {mappingSoil = 0;}

  if(mappingSoil > setPoint && !afterOn)
  {
    digitalWrite(relay, HIGH);
    Serial.println();
    Serial.println("ON");
    Serial.println();

    doit("From ESP", "Siram ON"); 
    delay(2000);
    afterOn = true;
    afterOff = false;
    
  }
  if(mappingSoil < setPoint && !afterOff)
  {
    digitalWrite(relay, LOW);
    Serial.println();
    Serial.println("OFF");
    Serial.println();

    doit("From ESP", "Siram OFF"); 
    delay(2000);
    afterOn = false;
    afterOff = true;
  }
  
  Serial.print(rawSoil); Serial.print("\t"); Serial.println(mappingSoil);
  delay(100);
}
