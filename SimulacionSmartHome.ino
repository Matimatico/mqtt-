//EMISOR
#include <WiFi.h>
#include <PubSubClient.h>

// Pines de pulsadores y LDR
#define living      15
#define cocina      2
#define dormitorio  4
#define toilet      5
#define todo        18  // LDR

// Estructura para guardar el estado de las luces
struct EstadoLuces {
  bool living = false;
  bool cocina = false;
  bool dormitorio = false;
  bool toilet = false;
  bool todas = false;
} estado;

// WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Credenciales WiFi y tópicos
const char* ssid = "washington";
const char* password = "washington";
const char* mqttServer = "broker.hivemq.com";

const char* topicLiving     = "serafini/casa/luz/living";
const char* topicCocina     = "serafini/casa/luz/cocina";
const char* topicDormitorio = "serafini/casa/luz/dormitorio";
const char* topicToilet     = "serafini/casa/luz/baño";
const char* topicTodas      = "serafini/casa/luz/todas";

void conectarWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Conectado!");
}

void setup() {
  Serial.begin(115200);

  // Configurar entradas
  pinMode(living, INPUT_PULLUP);
  pinMode(cocina, INPUT_PULLUP);
  pinMode(dormitorio, INPUT_PULLUP);
  pinMode(toilet, INPUT_PULLUP);
  pinMode(todo, INPUT);  // LDR como entrada analógica

  // Conexiones WiFi y MQTT
  conectarWiFi();
  client.setServer(mqttServer, 1883);

  while (!client.connected()) {
    Serial.println("Conectando al broker MQTT...");
    if (client.connect("PanelControl")) {
      Serial.println("Conectado al broker!");
    } else {
      Serial.print("Error: ");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

void loop() {
  client.loop();  // Mantiene la conexión viva

  // Pulsadores – lógica de cambio de estado
  if (digitalRead(living) == LOW) {
    estado.living = !estado.living;
    client.publish(topicLiving, estado.living ? "ON" : "OFF");
    delay(300); // debounce
  }

  if (digitalRead(cocina) == LOW) {
    estado.cocina = !estado.cocina;
    client.publish(topicCocina, estado.cocina ? "ON" : "OFF");
    delay(300);
  }

  if (digitalRead(dormitorio) == LOW) {
    estado.dormitorio = !estado.dormitorio;
    client.publish(topicDormitorio, estado.dormitorio ? "ON" : "OFF");
    delay(300);
  }

  if (digitalRead(toilet) == LOW) {
    estado.toilet = !estado.toilet;
    client.publish(topicToilet, estado.toilet ? "ON" : "OFF");
    delay(300);
  }

  // LDR – controlar todas las luces
  int luz = analogRead(todo);  // LDR
  static bool ultimaLectura = true;

  if (luz < 500 && ultimaLectura == false) {
    estado.todas = true;
    client.publish(topicTodas, "ON");
    ultimaLectura = true;
    delay(300);
  } else if (luz >= 500 && ultimaLectura == true) {
    estado.todas = false;
    client.publish(topicTodas, "OFF");
    ultimaLectura = false;
    delay(300);
  }
}


//RECEPTOR
#include <WiFi.h>
#include <PubSubClient.h>

// Pines para los LEDs
#define livingLED      15
#define cocinaLED      2
#define dormitorioLED  4
#define toiletLED      5

// WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Credenciales WiFi y broker MQTT
const char* ssid = "washington";
const char* password = "washington";
const char* mqttServer = "broker.hivemq.com";

// Tópicos a suscribirse
const char* topicLiving     = "serafini/casa/luz/living";
const char* topicCocina     = "serafini/casa/luz/cocina";
const char* topicDormitorio = "serafini/casa/luz/dormitorio";
const char* topicToilet     = "serafini/casa/luz/baño";
const char* topicTodas      = "serafini/casa/luz/todas";

// Función para conectar al WiFi
void conectarWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Conectado!");
}

// Función para encender/apagar LEDs
void controlarLEDs(const char* topic, const char* mensaje) {
  bool encender = strcmp(mensaje, "ON") == 0;

  if (strcmp(topic, topicLiving) == 0 || strcmp(topic, topicTodas) == 0) {
    digitalWrite(livingLED, encender ? HIGH : LOW);
  }

  if (strcmp(topic, topicCocina) == 0 || strcmp(topic, topicTodas) == 0) {
    digitalWrite(cocinaLED, encender ? HIGH : LOW);
  }

  if (strcmp(topic, topicDormitorio) == 0 || strcmp(topic, topicTodas) == 0) {
    digitalWrite(dormitorioLED, encender ? HIGH : LOW);
  }

  if (strcmp(topic, topicToilet) == 0 || strcmp(topic, topicTodas) == 0) {
    digitalWrite(toiletLED, encender ? HIGH : LOW);
  }
}

// Callback MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  char mensaje[length + 1];
  memcpy(mensaje, payload, length);
  mensaje[length] = '\0';

  Serial.print("Mensaje recibido en ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(mensaje);

  controlarLEDs(topic, mensaje);
}

void setup() {
  Serial.begin(115200);

  // Configurar LEDs como salida
  pinMode(livingLED, OUTPUT);
  pinMode(cocinaLED, OUTPUT);
  pinMode(dormitorioLED, OUTPUT);
  pinMode(toiletLED, OUTPUT);

  // Apagar todos los LEDs al inicio
  digitalWrite(livingLED, LOW);
  digitalWrite(cocinaLED, LOW);
  digitalWrite(dormitorioLED, LOW);
  digitalWrite(toiletLED, LOW);

  conectarWiFi();

  // Conectar MQTT
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Conectando al broker MQTT...");
    if (client.connect("ControladorLuces")) {
      Serial.println("Conectado al broker!");
      client.subscribe(topicLiving);
      client.subscribe(topicCocina);
      client.subscribe(topicDormitorio);
      client.subscribe(topicToilet);
      client.subscribe(topicTodas);
    } else {
      Serial.print("Error: ");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

void loop() {
  client.loop();  // Mantener conexión y recibir mensajes
}
