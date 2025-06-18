# mqtt-
trabajos realizados con mqtt
# Chat entre dos ESP32 usando MQTT

Este proyecto permite la comunicación entre **dos placas ESP32** a través de mensajes de texto utilizando el protocolo **MQTT**. Cada placa puede enviar y recibir mensajes simulando un chat, todo a través de una red Wi-Fi.

---

## Objetivo

- Conectarse a una red Wi-Fi.
- Usar un **broker MQTT público** para intercambiar mensajes.
- Cada ESP32 publica sus mensajes y se suscribe a los del otro.
- Los mensajes se envían y reciben con el formato: `<apellido>: <mensaje>`.

---

## Requisitos

### Software
- Arduino IDE
- Librerías:
  - `WiFi.h`
  - `PubSubClient.h` (instalable desde el Gestor de Bibliotecas)

### Hardware
- 2 placas ESP32
- 2 cables USB

---

## Configuración de red

- **SSID**: `washington`
- **Contraseña**: `washington`
- **Broker MQTT**: `broker.hivemq.com`
- **Puerto**: `1883`
- **Tópicos**:
  - ESP32 A publica en: `huergo/sistemas-embebidos/washington`
  - ESP32 A escucha: `huergo/sistemas-embebidos/serafini`
  - ESP32 B publica en: `huergo/sistemas-embebidos/serafini`
  - ESP32 B escucha: `huergo/sistemas-embebidos/washington`

---

## Estructura del código

Cada ESP32 sigue estos pasos:

1. Conectarse a la red Wi-Fi.
2. Conectarse al broker MQTT.
3. Suscribirse al tópico del otro ESP32.
4. Leer mensajes desde el Monitor Serial.
5. Publicar mensajes al tópico propio.
6. Escuchar nuevos mensajes (función `client.loop()`).
7. Mostrar en consola los mensajes recibidos.

---

## Ejecución

1. Subir el código al ESP32 A y ESP32 B (con las configuraciones adecuadas).
2. Abrir el Monitor Serial (velocidad 115200).
3. Escribir un mensaje en el Monitor Serial de uno, se mostrará en el otro.
4. El mensaje llegará con formato:  
   `washington: Hola, ¿cómo estás?`

---

## Archivos clave

- `esp32_chat_washington.ino`: Código para ESP32 A (usuario "washington")
- `esp32_chat_serafini.ino`: Código para ESP32 B (usuario "serafini") *(opcional, no incluido aquí)*

---

## Autor

Práctica para la materia **Sistemas Embebidos**, Escuela Huergo.  
Basado en el uso de MQTT y ESP32 con broker público.
