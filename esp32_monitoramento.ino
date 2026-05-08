/*
 * ============================================================
 *  Monitoramento IoT – ESP32 + DHT11 + HC-SR04 + MQTT
 *  Trabalho Prático – FIAP
 * ============================================================
 *  Sensores:
 *    - DHT11  → temperatura (°C) e umidade (%)  → GPIO 4
 *    - HC-SR04 → distância (cm)                 → TRIG: GPIO 12 / ECHO: GPIO 13
 *
 *  Tópicos MQTT:
 *    fiap/esp32/temperatura
 *    fiap/esp32/umidade
 *    fiap/esp32/distancia
 * ============================================================
 *  Bibliotecas necessárias (instalar via Library Manager):
 *    - PubSubClient (by Nick O'Leary)
 *    - DHT sensor library (by Adafruit)
 *    - Adafruit Unified Sensor (by Adafruit)
 * ============================================================
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ── Credenciais WiFi ──────────────────────────────────────────
const char* ssid     = "SEU_WIFI";
const char* password = "SUA_SENHA";

// ── Broker MQTT ───────────────────────────────────────────────
const char* mqtt_server = "broker.hivemq.com";
const int   mqtt_port   = 1883;

// ── Tópicos MQTT ─────────────────────────────────────────────
const char* TOPIC_TEMP = "fiap/esp32/temperatura";
const char* TOPIC_UMID = "fiap/esp32/umidade";
const char* TOPIC_DIST = "fiap/esp32/distancia";

// ── Pinos HC-SR04 ─────────────────────────────────────────────
#define TRIG_PIN 12
#define ECHO_PIN 13

// ── DHT11 ────────────────────────────────────────────────────
#define DHT_PIN  4
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// ── Objetos ───────────────────────────────────────────────────
WiFiClient   espClient;
PubSubClient client(espClient);

// ── Intervalo de leitura (ms) ────────────────────────────────
const unsigned long INTERVALO = 3000;
unsigned long ultimaLeitura   = 0;

// =============================================================
//  FUNÇÕES AUXILIARES
// =============================================================

void setup_wifi() {
  Serial.print("\n[WiFi] Conectando a ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] Conectado! IP: " + WiFi.localIP().toString());
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("[MQTT] Conectando...");
    // Gera um clientId único para evitar conflito no broker público
    String clientId = "ESP32-FIAP-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println(" conectado! ClientID: " + clientId);
    } else {
      Serial.print(" falhou (rc=");
      Serial.print(client.state());
      Serial.println("). Tentando em 3s...");
      delay(3000);
    }
  }
}

float lerDistancia() {
  // Pulso TRIG
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Mede duração do ECHO (timeout 30ms → ~5m máx)
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duracao == 0) return -1.0;          // sem retorno = fora do alcance

  float distancia = duracao * 0.034 / 2.0;
  return distancia;
}

void publicar(const char* topico, float valor, int casas = 1) {
  char buf[16];
  dtostrf(valor, 4, casas, buf);          // converte float → string
  client.publish(topico, buf);
  Serial.printf("[MQTT] %s → %s\n", topico, buf);
}

// =============================================================
//  SETUP
// =============================================================
void setup() {
  Serial.begin(115200);

  // Pinos ultrassônico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // DHT
  dht.begin();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("[Sistema] Pronto para monitorar!");
}

// =============================================================
//  LOOP PRINCIPAL
// =============================================================
void loop() {
  // Reconecta se necessário
  if (!client.connected()) reconnect_mqtt();
  client.loop();

  unsigned long agora = millis();
  if (agora - ultimaLeitura >= INTERVALO) {
    ultimaLeitura = agora;

    // ── Leitura DHT11 ──────────────────────────────────────
    float temperatura = dht.readTemperature();
    float umidade     = dht.readHumidity();

    if (!isnan(temperatura)) {
      publicar(TOPIC_TEMP, temperatura, 1);
    } else {
      Serial.println("[DHT] Erro ao ler temperatura");
    }

    if (!isnan(umidade)) {
      publicar(TOPIC_UMID, umidade, 1);
    } else {
      Serial.println("[DHT] Erro ao ler umidade");
    }

    // ── Leitura HC-SR04 ────────────────────────────────────
    float distancia = lerDistancia();
    if (distancia > 0 && distancia < 400) {
      publicar(TOPIC_DIST, distancia, 1);
    } else {
      Serial.println("[HC-SR04] Sem objeto detectado ou fora do alcance");
    }

    Serial.println("─────────────────────────────");
  }
}
