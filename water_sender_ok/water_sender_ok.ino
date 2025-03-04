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

#define TRIG_PIN 18  
#define ECHO_PIN 19  
#define TIMEOUT 200000  
#define NUM_SAMPLES 10  

uint8_t receiverMAC[] = {0xA4, 0xCF, 0x12, 0x1D, 0x96, 0x0C};  // Replace with Receiver MAC

float distanceSamples[NUM_SAMPLES] = {0};  
int sampleIndex = 0;
float lastValidDistance = -1;
unsigned long lastUpdateTime = 0;  
const int UPDATE_INTERVAL = 500;  // ✅ OLED update every 500ms

typedef struct {
    float distance;
} DistanceMessage;

DistanceMessage msg;
String sendStatus = "Waiting";  // ✅ Store last send status

// ✅ ESP-NOW Send Callback
void sendCallback(const uint8_t *mac, esp_now_send_status_t status) {
    sendStatus = (status == ESP_NOW_SEND_SUCCESS) ? "Send: OK" : "Send: Failed";
    Serial.println(sendStatus);
}

// ✅ Function to Get Distance
float getDistance() {
    long duration;
    float distance_cm;

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(5);  
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseInLong(ECHO_PIN, HIGH, TIMEOUT);  

    if (duration == 0) return -1;
    distance_cm = (duration * 0.0343) / 2;  

    if (distance_cm > 400 || distance_cm < 2) return -1;  

    return distance_cm;
}

// ✅ Function to Compute Median (Removes Outliers)
float getMedian(float arr[], int size) {
    float sorted[size];
    memcpy(sorted, arr, size * sizeof(float));
    std::sort(sorted, sorted + size);
    return sorted[size / 2];  
}

// ✅ Function to Get a Stable Filtered Distance
float getFilteredDistance() {
    float rawDistance = getDistance();
    if (rawDistance == -1) {
        Serial.println("No valid distance detected! Using last known distance.");
        return lastValidDistance;
    }

    lastValidDistance = rawDistance;
    distanceSamples[sampleIndex] = rawDistance;
    sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;

    return getMedian(distanceSamples, NUM_SAMPLES);
}

void setup() {
    Serial.begin(115200);
    
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // ✅ Initialize OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("OLED init failed!");
        while (true);
    }
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.print("Ready...");
    display.display();
    delay(2000);

    // ✅ Initialize ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed!");
        return;
    }
    
    esp_now_register_send_cb(sendCallback);
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer!");
        return;
    }
}

void loop() {
    float distance_cm = getFilteredDistance();
    
    Serial.print("Sending Distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");

    msg.distance = distance_cm;
    esp_now_send(receiverMAC, (uint8_t *)&msg, sizeof(msg));

    // ✅ Update OLED every UPDATE_INTERVAL milliseconds
    if (millis() - lastUpdateTime >= UPDATE_INTERVAL) {
        lastUpdateTime = millis();

        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(10, 10);
       // display.print("Dist: ");
        display.print(distance_cm, 1);
        display.print(" cm");

        // ✅ Display Send Status
        display.setCursor(10, 40);
        display.print(sendStatus);

        display.display();
    }

    delay(500);
}
