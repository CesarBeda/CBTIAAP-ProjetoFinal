#include <Arduino.h>
#include <NewPing.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// --- Bibliotecas para o Display OLED ---
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Definições dos Pinos do HC-SR04 ---
#define TRIGGER_PIN  23
#define ECHO_PIN     19
#define MAX_DISTANCE 200 // Distância máxima que o sensor pode medir em cm

// --- DEFINIÇÕES PARA O TANQUE CILÍNDRICO (1000 LITROS) ---
#define TANK_HEIGHT_CM       76.0 // Altura física do tanque até o limite de 1000L
#define TANK_DIAMETER_CM     129.44
#define TANK_RADIUS_CM       (TANK_DIAMETER_CM / 2.0)

// Distância do sensor ultrassônico até o limite de 1000L dentro do tanque
#define SENSOR_MOUNT_OFFSET_CM 10.0 // Se o sensor lê 10cm, significa que o tanque tem 1000L

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// --- Configurações de tempo para leitura ---
#define PING_INTERVAL 60000 // 15 segundos entre as leituras

unsigned long lastPingTime = 0;

// --- DADOS DO WI-FI (para Wokwi) ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// --- DADOS DO THINGSPEAK ---
unsigned long myChannelNumber = SEU_CHANNEL_ID;     // SUBSTITUA PELO SEU CHANNEL ID
const char* myWriteAPIKey = "SUBSTITUA_PELA_SUA_WRITE_API_KEY"; // SUBSTITUA PELA SUA WRITE API KEY

WiFiClient client;

// --- DEFINIÇÕES PARA O DISPLAY OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

// OLED I2C: Pinos 21 (SDA) e 22 (SCL) - Padrões do ESP32 e funcionais
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- DEFINIÇÕES DOS PINOS DOS LEDs (TODOS NO LADO ESQUERDO) ---
#define LED_AMARELO_PIN  13
#define LED_VERDE_PIN    12
#define LED_AZUL_PIN     14
#define LED_VERMELHO_PIN 27

// --- Novos Limiares de Nível para LEDs e Status (em porcentagem) ---
#define NIVEL_BAIXO_MAX_PERCENT        35.0 // Amarelo: 0% a 35%
#define NIVEL_OK_MIN_PERCENT           35.1 // Verde: > 35% a 70%
#define NIVEL_OK_MAX_PERCENT           70.0
#define NIVEL_CHEIO_MIN_PERCENT        70.1 // Azul: > 70% a 100%
#define NIVEL_ACIMA_LIMITE_MIN_PERCENT 100.1 // Vermelho: > 100% (acima do ideal)

// Função para controlar os LEDs com base no nível
void setLEDs(float water_level_percent, float distance_cm_raw) {
  digitalWrite(LED_AMARELO_PIN, LOW);
  digitalWrite(LED_VERDE_PIN, LOW);
  digitalWrite(LED_AZUL_PIN, LOW);
  digitalWrite(LED_VERMELHO_PIN, LOW);

  // Se a leitura for 0 (fora de alcance), desliga tudo.
  if (distance_cm_raw == 0) {
    // Todos já estão LOW, não faz nada
    return;
  }

  // Lógica para ligar o LED apropriado
  if (water_level_percent >= NIVEL_ACIMA_LIMITE_MIN_PERCENT) {
    // Isso ocorre quando distance_cm_raw < SENSOR_MOUNT_OFFSET_CM
    digitalWrite(LED_VERMELHO_PIN, HIGH);
  } else if (water_level_percent >= NIVEL_CHEIO_MIN_PERCENT && water_level_percent <= 100.0) {
    digitalWrite(LED_AZUL_PIN, HIGH);
  } else if (water_level_percent >= NIVEL_OK_MIN_PERCENT && water_level_percent <= NIVEL_OK_MAX_PERCENT) {
    digitalWrite(LED_VERDE_PIN, HIGH);
  } else if (water_level_percent >= 0.0 && water_level_percent <= NIVEL_BAIXO_MAX_PERCENT) {
    digitalWrite(LED_AMARELO_PIN, HIGH);
  }
}

// Função para retornar a string de status
String getStatusString(float water_level_percent, float distance_cm_raw) {
  if (distance_cm_raw == 0) { // Fora de alcance
    return "Status: INDEFINIDO";
  } else if (water_level_percent >= NIVEL_ACIMA_LIMITE_MIN_PERCENT) {
    return "Status: ACIMA DO LIMITE";
  } else if (water_level_percent >= NIVEL_CHEIO_MIN_PERCENT && water_level_percent <= 100.0) {
    return "Status: CHEIO";
  } else if (water_level_percent >= NIVEL_OK_MIN_PERCENT && water_level_percent <= NIVEL_OK_MAX_PERCENT) {
    return "Status: NIVEL OK";
  } else if (water_level_percent >= 0.0 && water_level_percent <= NIVEL_BAIXO_MAX_PERCENT) {
    return "Status: NIVEL BAIXO";
  }
  return "Status: INDEFINIDO"; // Fallback para casos não cobertos
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Monitoramento de Nivel de Agua com HC-SR04...");
  Serial.print("Altura do Tanque (1000L): ");
  Serial.print(TANK_HEIGHT_CM);
  Serial.println(" cm");
  Serial.print("Raio do Tanque: ");
  Serial.print(TANK_RADIUS_CM);
  Serial.println(" cm");
  Serial.print("Sensor Offset ate 1000L: ");
  Serial.print(SENSOR_MOUNT_OFFSET_CM);
  Serial.println(" cm");
  Serial.println("----------------------------------------");

  // --- Configura pinos dos LEDs como OUTPUT ---
  pinMode(LED_AMARELO_PIN, OUTPUT);
  pinMode(LED_VERDE_PIN, OUTPUT);
  pinMode(LED_AZUL_PIN, OUTPUT);
  pinMode(LED_VERMELHO_PIN, OUTPUT);

  // Garante que todos os LEDs estão desligados no início
  setLEDs(-1.0, 999.0); // Valores que não acionam nenhum LED

  // --- Inicializando o Display OLED ---
  Wire.begin(21, 22); // SDA, SCL - Pinos corretos para I2C no ESP32
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Falha ao alocar SSD1306 ou encontrar OLED"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // --- Conectando ao Wi-Fi ---
  display.setCursor(0,0);
  display.println("Conectando WiFi...");
  display.display();

  Serial.print("Conectando ao WiFi ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado!");
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi Conectado!");
  display.println(WiFi.localIP());
  display.display();
  delay(2000);
  display.clearDisplay();

  ThingSpeak.begin(client);
}

void loop() {
  if (millis() - lastPingTime >= PING_INTERVAL) {
    lastPingTime = millis();

    unsigned long duration = sonar.ping();
    float distance_cm_raw = sonar.convert_cm(duration);

    display.clearDisplay();
    display.setCursor(0,0);

    Serial.print("Distancia lida pelo sensor: ");
    if (distance_cm_raw == 0 || distance_cm_raw > (TANK_HEIGHT_CM + SENSOR_MOUNT_OFFSET_CM)) {
      Serial.println("Fora de alcance / Sem Eco / Nivel abaixo do fundo"); // Mensagem atualizada
      Serial.println("Nivel de Agua: N/A"); // Exibe N/A
      Serial.println("Volume Atual: N/A");   // Exibe N/A
      
      display.println("Fora de Alcance"); // No OLED, pode manter a mensagem mais curta
      display.println("Nivel: N/A");
      display.println("Volume: N/A");
      // Importante: Passa -1.0 para water_level_percent para que getStatusString retorne "INDEFINIDO"
      display.println(getStatusString(-1.0, distance_cm_raw));
      display.display();

      // Passa -1.0 para water_level_percent para que setLEDs não acenda nenhum LED para este estado "indefinido"
      // e 0 para distance_cm_raw para acionar a lógica de desligar tudo.
      setLEDs(-1.0, 0); 
      ThingSpeak.setField(1, 0); // Envia 0 para ThingSpeak (volume)
      ThingSpeak.setField(2, 0); // Envia 0 para ThingSpeak (percentual)
    } else {
      Serial.print(distance_cm_raw);
      Serial.println(" cm");
      display.print("Dist Sensor: ");
      display.print(distance_cm_raw, 0);
      display.println(" cm");

      // Calcula o nível de água em cm em relação ao fundo do tanque
      float water_level_cm = (TANK_HEIGHT_CM + SENSOR_MOUNT_OFFSET_CM) - distance_cm_raw;
      
      // Ajusta water_level_cm para nunca ser negativo para cálculo de volume e exibição,
      // mas permite que o percentual e status indiquem acima de 100%.
      float effective_water_level_cm = max(0.0F, water_level_cm);

      float volume_cm3 = PI * (TANK_RADIUS_CM * TANK_RADIUS_CM) * effective_water_level_cm;
      float volume_liters = volume_cm3 / 1000.0;
      float water_level_percent = (water_level_cm / TANK_HEIGHT_CM) * 100.0; // Usa water_level_cm (pode ser > TANK_HEIGHT_CM) para percentual

      // --- Exibe no Serial Monitor ---
      Serial.print("Nivel de Agua: ");
      Serial.print(effective_water_level_cm, 0); // Exibe o nível ajustado para não ser negativo
      Serial.print(" cm (");
      Serial.print(water_level_percent, 1);
      Serial.println("%)");

      Serial.print("Volume Atual: ");
      Serial.print(volume_liters, 2);
      Serial.println(" Litros");
      
      String statusStr = getStatusString(water_level_percent, distance_cm_raw);
      Serial.println(statusStr); // Exibe o Status
      Serial.println(); // Pula uma linha

      // --- Exibe no Display OLED ---
      display.print("Nivel: ");
      display.print(effective_water_level_cm, 0); // Exibe o nível ajustado para não ser negativo
      display.print("cm (");
      display.print(water_level_percent, 0);
      display.println("%)");

      display.print("Vol: ");
      display.print(volume_liters, 1);
      display.println(" Litros");

      display.println(statusStr);
      display.display();

      setLEDs(water_level_percent, distance_cm_raw);

      // --- ENVIANDO PARA THINGSPEAK ---
      ThingSpeak.setField(1, volume_liters);
      ThingSpeak.setField(2, water_level_percent);

      int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      if (httpCode == 200) {
        Serial.println("Dados enviados para ThingSpeak com sucesso.");
      } else {
        Serial.print("Falha ao enviar dados para ThingSpeak. Codigo de erro: ");
        Serial.println(httpCode);
      }
    }
    Serial.println("----------------------------------------"); // Linha movida para o final do loop principal
  }
}