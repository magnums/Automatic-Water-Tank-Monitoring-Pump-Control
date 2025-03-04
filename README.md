This project is designed to monitor a water tank level using an ultrasonic sensor (AJ-SR04M) and control a pump based on the water level. The system consists of two Wemos D1 R32 (ESP32) boards:

Sender: Reads the water level and sends the data wirelessly via ESP-NOW.
Receiver: Receives the data, displays it on an OLED screen, and controls a relay to turn the pump ON/OFF.
📌 Features
✅ Wireless Data Transmission: Uses ESP-NOW for fast, efficient communication between two ESP32s.
✅ Ultrasonic Water Level Measurement: Reads distance from water surface to sensor and calculates water percentage.
✅ OLED Display with Tank Graphics: Visually represents water level inside a tank.
✅ Relay-Based Pump Control: Turns the pump ON/OFF based on water level using a hysteresis system to prevent rapid switching.
✅ Stable Pump Control (Hysteresis):

Pump turns ON at 100 cm and stays ON until 25 cm.
Pump turns OFF at 25 cm and stays OFF until 100 cm.
✅ Failsafe Mechanism: If ESP-NOW connection is lost, the last known water level is maintained.
📡 System Overview
1️⃣ Sender (Wemos D1 R32 + Ultrasonic Sensor)
Reads water level from AJ-SR04M waterproof ultrasonic sensor.
Displays current distance & send status on OLED screen.
Sends water level data to the receiver via ESP-NOW.
2️⃣ Receiver (Wemos D1 R32 + OLED + Relay)
Receives the distance from the sender.
Converts distance into a water level percentage.
Displays a small tank graphic on the OLED to represent water level.
Controls the relay to turn the pump ON/OFF based on water level.
💡 How It Works (Water Tank Filling Process)
Water Level Drops Below 100 cm → Pump turns ON (Filling starts).
Water Level Rises to 25 cm → Pump turns OFF (Tank is full).
Water is Used & Level Drops Again → Pump stays OFF until 100 cm is reached.
Cycle Repeats Automatically
This prevents unnecessary ON/OFF switching and ensures efficient pump operation.

📌 Hardware Components
Component	Quantity	Description
Wemos D1 R32 (ESP32)	2	One for sender, one for receiver
AJ-SR04M Ultrasonic Sensor	1	Waterproof distance measurement sensor
0.96" OLED Display	1	Displays water level, distance & pump status
Relay Module	1	Controls the water pump
5V Power Adapter	1	Powers the ESP32 and relay
100µF Capacitor	1	Stabilizes ultrasonic sensor voltage
Jumper Wires	Several	For wiring connections
📌 Code Overview
📜 Sender Code (ESP32 + Ultrasonic Sensor)
Reads distance from ultrasonic sensor.
Displays distance & send status on OLED screen.
Sends water level data to receiver via ESP-NOW.
If send fails, displays "Send: Failed" on OLED.
📜 Receiver Code (ESP32 + OLED + Relay)
Receives distance from sender.
Converts it into a water level percentage.
Displays a small tank graphic on OLED with dynamic water level.
Turns the relay ON/OFF based on water level thresholds.
Uses hysteresis to prevent rapid relay switching.
📌 Installation & Setup
1️⃣ Flash the sender code to the Wemos D1 R32 (sender).
2️⃣ Flash the receiver code to the Wemos D1 R32 (receiver).
3️⃣ Find the MAC address of the receiver and replace it in the sender code (receiverMAC[]).
4️⃣ Power both ESP32s, and the system will start working!

🚀 Conclusion
This project automates water tank monitoring using ESP32, ultrasonic sensors, and OLED graphics to provide a clear and visual representation of water levels while efficiently controlling the pump. The use of ESP-NOW makes it a fast and WiFi-independent solution.

