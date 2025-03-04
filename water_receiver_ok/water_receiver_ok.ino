 #include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_now.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64  
#define OLED_RESET -1     
#define SCREEN_ADDRESS 0x3C  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define RELAY_PIN 5  // ✅ Relay connected to GPIO 5

// ✅ Water level thresholds with Hysteresis
const float TURN_ON_DISTANCE = 100.0;  // ✅ Pump turns ON at 100 cm
const float TURN_OFF_DISTANCE = 25.0;  // ✅ Pump turns OFF at 25 cm

typedef struct {
    float distance;
} DistanceMessage;

DistanceMessage msg;
bool relayState = false;  // ✅ Stores relay state (false = OFF, true = ON)

// ✅ Function to map distance to water level percentage
int calculateWaterLevel(float distance) {
    int level = map(distance, TURN_ON_DISTANCE, TURN_OFF_DISTANCE, 0, 100);
    return constrain(level, 0, 100);  // Ensure level stays between 0% and 100%
}

// ✅ ESP-NOW Receive Callback
void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    memcpy(&msg, incomingData, sizeof(msg));
    Serial.print("Received Distance: ");
    Serial.print(msg.distance);
    Serial.println(" cm");

    // ✅ Hysteresis for Relay Control
    if (!relayState && msg.distance >= TURN_ON_DISTANCE) {
        digitalWrite(RELAY_PIN, LOW);  // Turn ON relay (Pump ON)
        relayState = true;
        Serial.println("Water Low! Turning ON Pump");
    } 
    else if (relayState && msg.distance <= TURN_OFF_DISTANCE) {
        digitalWrite(RELAY_PIN, HIGH);  // Turn OFF relay (Pump OFF)
        relayState = false;
        Serial.println("Water Full! Turning OFF Pump");
    }

    // ✅ Calculate Water Level %
    int waterLevel = calculateWaterLevel(msg.distance);

    // ✅ Update OLED Display
    display.clearDisplay();

    // ✅ Draw Tank Outline
    display.drawRect(90, 10, 30, 40, WHITE);  // Tank border

    // ✅ Fill Tank Based on Water Level
    int fillHeight = map(waterLevel, 0, 100, 0, 38);  // Scale water level to tank height
    display.fillRect(92, 48 - fillHeight, 26, fillHeight, WHITE);  // Water inside tank

  // ✅ Show Exact Distance (NEW FEATURE!)
    display.setCursor(10, 5);
    display.print("Dist: ");
    display.print(msg.distance, 1);
    display.print("cm"); 

    // ✅ Show Water Level Percentage
    display.setTextSize(1);
    display.setCursor(10, 20);
    display.print("Water: ");
    display.print(waterLevel);
    display.print("%");


    // ✅ Display Pump Status
    display.setCursor(10, 40);
    display.print("Pump: ");
    display.print(relayState ? "ON" : "OFF");

    display.display();
}

void setup() {
    Serial.begin(115200);

    // ✅ Initialize OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("OLED init failed!");
        while (true);
    }
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.print("Waiting...");
    display.display();

    // ✅ Set up Relay
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);  // Default relay state is OFF

    // ✅ Initialize ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed!");
        return;
    }

    esp_now_register_recv_cb(onReceive);
}

void loop() {
    // Nothing needed in loop, ESP-NOW handles receiving
}
