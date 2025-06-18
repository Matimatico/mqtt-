#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

void conectarWiFi() {
  WiFi.begin("washington", "washington");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(". . . ");
    delay(1000);
  }
  Serial.println("Conectado");
}

void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  conectarWiFi();


  client.setServer("broker.hivemq.com", 1883);

  client.setCallback(callback);

  while (!client.connect("washington")) { 
    Serial.println("esperando...");
    delay(3000);
  }

  client.subscribe("huergo/sistemas-embebidos/serafini");
}

void loop() {
  client.loop();

  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();

    String mensaje = "washington: " + msg;
    client.publish("huergo/sistemas-embebidos/washington", mensaje.c_str());
  }
}
