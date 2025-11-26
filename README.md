# 🌤️ Weather Station ESP32 v2.0

Professional weather station based on ESP32 with TFT display, multiple sensors, and **JSON HTTP API** for data integration.

![Version](https://img.shields.io/badge/version-2.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Author](https://img.shields.io/badge/author-Marautech-purple)
## 📸 Preview 1.8" Version

![1,8 TFT](https://github.com/user-attachments/assets/1d0a9575-d50f-4816-a22c-2569eb90cfec)

## ✨ Features

- **Multi-sensor support**: 2x DS18B20 (temperature), AHT20 (humidity), BMP280 (pressure)
- **TFT Display**: 1.8" ST7735 color display with modern minimalist UI
- **3 Screen Navigation**: Temperature, Trends Graph, System Info
- **HTTP JSON API**: Real-time data export via REST API
- **Web Dashboard**: Built-in HTML interface for remote monitoring
- **WiFi Connectivity**: Automatic connection and IP display
- **Data Logging**: 288 data points history (24h at 5-minute intervals)
- **Dew Point Calculation**: Automatic dew point computation
- **Color-coded Values**: Visual feedback for temperature readings
- **Professional UI**: Clean, modern interface optimized for readability

## 🔧 Hardware

### Main Components

- **Microcontroller**: ESP32 DevKit v1
- **Display**: TFT 1.8" LCD ST7735 (SPI)
- **Temperature Sensors**: 2x DS18B20 (1-Wire) - Indoor & Outdoor
- **Humidity Sensor**: AHT20 (I2C)
- **Pressure Sensor**: BMP280 (I2C)
- **LED Indicator**: GPIO26

### Requirements

- ESP32 Development Board
- ST7735 TFT Display Module (128x160)
- 2x DS18B20 Temperature Sensors
- AHT20 Temperature & Humidity Sensor
- BMP280 Barometric Pressure Sensor
- Resistors: 1x 4.7kΩ (pull-up for DS18B20)
- Breadboard and jumper wires
- USB cable for power and programming

## 📊 Sensors

### DS18B20 Temperature Sensors (x2)
- **Interface**: 1-Wire
- **Range**: -55°C to +125°C
- **Accuracy**: ±0.5°C
- **Usage**: Indoor and outdoor temperature measurement
- **Advantages**: Waterproof versions available, multiple sensors on one wire

### AHT20 Temperature & Humidity Sensor
- **Interface**: I2C
- **Temperature Range**: -40°C to +85°C
- **Humidity Range**: 0% to 100% RH
- **Accuracy**: ±2% RH, ±0.3°C
- **Features**: Factory calibrated, low power consumption

### BMP280 Barometric Pressure Sensor
- **Interface**: I2C
- **Pressure Range**: 300 to 1100 hPa
- **Accuracy**: ±1 hPa
- **Temperature Range**: -40°C to +85°C
- **Features**: High precision, temperature compensation

## 📡 Pinout

| Component | Pin | ESP32 GPIO | Notes |
|-----------|-----|------------|-------|
| **TFT ST7735** |
| VCC | - | 3.3V | Power supply |
| GND | - | GND | Ground |
| CS | - | GPIO5 | Chip Select (SPI) |
| RESET | - | GPIO4 | Display Reset |
| DC/A0 | - | GPIO2 | Data/Command Select |
| SDA/MOSI | - | GPIO23 | SPI Data Out |
| SCK | - | GPIO18 | SPI Clock |
| LED | - | 3.3V | Backlight (always on) |
| **DS18B20 (x2)** |
| DATA | - | GPIO15 | 1-Wire Data Bus |
| VCC | - | 3.3V | Power supply |
| GND | - | GND | Ground |
| Pull-up | - | 4.7kΩ | Between DATA and 3.3V |
| **AHT20** |
| VCC | - | 3.3V | Power supply |
| GND | - | GND | Ground |
| SDA | - | GPIO21 | I2C Data |
| SCL | - | GPIO22 | I2C Clock |
| **BMP280** |
| VCC | - | 3.3V | Power supply |
| GND | - | GND | Ground |
| SDA | - | GPIO21 | I2C Data (shared) |
| SCL | - | GPIO22 | I2C Clock (shared) |
| **Controls & Indicators** |
| BOOT Button | - | GPIO0 | Screen navigation (built-in) |
| LED Indicator | - | GPIO26 | Status LED |

## 🌐 Network Configuration

### WiFi Settings

Update WiFi credentials in the code:

```cpp
const char* WIFI_SSID = "your_wifi_name";
const char* WIFI_PASSWORD = "your_wifi_password";
const int HTTP_PORT = 80;
```

### API Endpoints

After connecting to WiFi, the station exposes two endpoints:

**JSON Data API:**
```
http://[ESP32_IP]/data
```
Returns real-time sensor data in JSON format.

**Web Dashboard:**
```
http://[ESP32_IP]/
```
HTML interface with auto-refreshing values.

### JSON Response Format

```json
{
  "station": "WeatherStation",
  "version": "2.0",
  "uptime": 12345,
  "temperature": {
    "indoor": 22.5,
    "outdoor": 18.3,
    "unit": "celsius"
  },
  "environment": {
    "humidity": 45.2,
    "pressure": 1013.2,
    "dewPoint": 10.8
  },
  "sensors": {
    "ds18b20": true,
    "bmp280": true,
    "aht20": true
  },
  "network": {
    "wifi": true,
    "ip": "192.168.1.123",
    "rssi": -45
  },
  "statistics": {
    "dataPoints": 150,
    "maxPoints": 288
  }
}
```

## 🚀 Installation

### 1. Install Arduino IDE

Download and install [Arduino IDE](https://www.arduino.cc/en/software) version 2.0 or later (1.8.x also supported).

### 2. Add ESP32 Board Support

1. Open Arduino IDE
2. Go to **File** → **Preferences**
3. Add this URL to **Additional Boards Manager URLs**:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools** → **Board** → **Boards Manager**
5. Search for "ESP32" and install **esp32 by Espressif Systems**
6. Select **Tools** → **Board** → **ESP32 Arduino** → **ESP32 Dev Module**

### 3. Install Required Libraries

In Arduino IDE, go to **Sketch** → **Include Library** → **Manage Libraries** and install:

#### Display Libraries
- **Adafruit GFX Library** (by Adafruit) - v1.11.0 or later
- **Adafruit ST7735 and ST7789 Library** (by Adafruit) - v1.10.0 or later

#### Sensor Libraries
- **OneWire** (by Paul Stoffregen) - v2.3.7 or later
- **DallasTemperature** (by Miles Burton) - v3.9.0 or later
- **Adafruit BMP280 Library** (by Adafruit) - v2.6.0 or later
- **Adafruit AHTX0** (by Adafruit) - v2.0.0 or later

#### Network & JSON
- **ArduinoJson** (by Benoit Blanchon) - v6.21.0 or later
- **WiFi** (built-in with ESP32)
- **WebServer** (built-in with ESP32)

### 4. Hardware Setup

1. **Connect TFT Display** to ESP32 via SPI pins
2. **Wire DS18B20 sensors** with 4.7kΩ pull-up resistor on DATA line
3. **Connect AHT20 and BMP280** to I2C bus (SDA/SCL)
4. **Double-check all connections** before powering on
5. **Power ESP32** via USB (5V)

### 5. Upload Code

1. Clone or download this repository
2. Open `Weather_Station_v2.ino` in Arduino IDE
3. Update WiFi credentials in the code
4. Connect ESP32 to your computer via USB
5. Select the correct COM port: **Tools** → **Port**
6. Click **Upload** button (or press Ctrl+U)
7. Wait for compilation and upload to complete
8. Open Serial Monitor (115200 baud) to view IP address

## 🎮 Usage

### Power On Sequence

After uploading the code, the weather station will:

1. **Display boot screen** with logo and version
2. **Initialize sensors** (BMP280 → AHT20 → DS18B20)
3. **Connect to WiFi** and obtain IP address
4. **Show IP address** on display for 5 seconds
5. **Start HTTP server** on port 80
6. **Begin sensor readings** every 5 seconds
7. **Switch to main screen** automatically

### Navigation

Press the **BOOT button** (GPIO0) on ESP32 to cycle through screens:

- **Screen 1 - Main**: Indoor/Outdoor Temperature, Humidity, Pressure, Dew Point, Data Log Count
- **Screen 2 - Trends**: Temperature and Pressure graphs (24-hour history)
- **Screen 3 - System Info**: Sensor status, WiFi status, IP address, firmware version

### Accessing Data

#### Via Serial Monitor
Open Serial Monitor at **115200 baud** to view:
```
=== Weather Station v2.0 ===
Init BMP280...
Init AHT20...
Init DS18B20...
Connecting WiFi...
Starting HTTP Server...
=== SYSTEM READY ===
IP Address: 192.168.1.123
JSON API: http://192.168.1.123/data
Web Dashboard: http://192.168.1.123/
```

#### Via Web Browser
1. Note the IP address from Serial Monitor or TFT display
2. Open browser and navigate to `http://[IP_ADDRESS]/`
3. View real-time data with auto-refresh (every 5 seconds)

#### Via API Call
Use curl, Postman, or any HTTP client:
```bash
curl http://192.168.1.123/data
```

#### Integration Examples

**Python:**
```python
import requests
import json

response = requests.get('http://192.168.1.123/data')
data = response.json()
print(f"Indoor: {data['temperature']['indoor']}°C")
print(f"Outdoor: {data['temperature']['outdoor']}°C")
print(f"Humidity: {data['environment']['humidity']}%")
```

**Node.js:**
```javascript
const axios = require('axios');

axios.get('http://192.168.1.123/data')
  .then(response => {
    console.log('Temperature:', response.data.temperature);
    console.log('Humidity:', response.data.environment.humidity);
  });
```

**Home Assistant:**
```yaml
sensor:
  - platform: rest
    resource: http://192.168.1.123/data
    name: Weather Station
    json_attributes:
      - temperature
      - environment
      - sensors
    value_template: '{{ value_json.temperature.indoor }}'
```

## 🖥️ Screens

### Screen 1: Main Dashboard
**Display Elements:**
- Header: "WEATHER STATION v2.0" with WiFi indicator
- Large temperature displays (indoor/outdoor) with color coding
- Humidity percentage with cyan accent
- Barometric pressure in hPa
- Dew point temperature in orange
- Data log count (current/maximum)

**Color Coding:**
- 🔵 Cyan: Cool (< 15°C)
- 🟢 Green: Normal (15-22°C)
- 🟡 Orange: Warm (22-26°C)
- 🔴 Red: Hot (> 26°C)

### Screen 2: Trends Graph
**Display Elements:**
- Temperature trend graph (24-hour history)
- Pressure trend graph
- Automatic scaling based on min/max values
- Visual representation of data patterns

**Features:**
- Stores up to 288 data points (5-minute intervals)
- Smooth line graphs with color coding
- Min/Max values displayed on Y-axis

### Screen 3: System Info
**Display Elements:**
- Sensor status indicators (OK/ERROR)
  - DS18B20: Temperature sensors
  - BMP280: Pressure sensor
  - AHT20: Humidity sensor
- WiFi status (OK/OFF)
- IP Address display
- JSON endpoint information
- Firmware version

**Status Colors:**
- 🟢 Green "OK": Sensor detected and working
- 🔴 Red "ERROR": Sensor not detected or failed

## 🔧 Troubleshooting

### Display Issues

**Problem: Colored bars on edges**
```cpp
// Try different initialization:
tft.initR(INITR_BLACKTAB);   // Default
// or
tft.initR(INITR_GREENTAB);   // Alternative
// or
tft.initR(INITR_REDTAB);     // Another option
```

**Problem: Display is blank**
- Check VCC connected to 3.3V (NOT 5V!)
- Verify all SPI connections
- Test with different CS/DC/RST pin assignments

### Sensor Issues

**DS18B20 Not Detected:**
- ✅ Verify 4.7kΩ pull-up resistor between DATA and 3.3V
- ✅ Check DATA pin is GPIO15
- ✅ Test with a single sensor first
- ✅ Try different resistor values (2.2kΩ - 10kΩ)
- ✅ Ensure sensors are powered (3.3V)

**AHT20 Not Responding:**
- ✅ Check I2C connections (SDA=GPIO21, SCL=GPIO22)
- ✅ Try I2C scanner sketch to detect address
- ✅ Verify 3.3V power supply
- ✅ Check for loose connections

**BMP280 Not Found:**
- ✅ Code checks both 0x76 and 0x77 addresses
- ✅ Verify I2C shared with AHT20 properly
- ✅ Try connecting alone to test
- ✅ Some modules need 5V (check datasheet)

### WiFi Issues

**Cannot Connect:**
- ✅ Verify SSID and password in code
- ✅ Check 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- ✅ Ensure router allows new devices
- ✅ Check Serial Monitor for error messages
- ✅ Try moving ESP32 closer to router

**IP Not Displayed:**
- ✅ Wait 5 seconds after boot for IP screen
- ✅ Press BOOT button to cycle to System Info screen
- ✅ Check Serial Monitor (115200 baud)
- ✅ Ensure WiFi connected successfully

### HTTP API Issues

**Cannot Access Web Interface:**
- ✅ Verify IP address is correct
- ✅ Check that device is on same network
- ✅ Try ping to ESP32 IP
- ✅ Disable firewall temporarily
- ✅ Use `http://` not `https://`

**JSON Data Not Updating:**
- ✅ Refresh browser or API call
- ✅ Check sensor readings on display
- ✅ Verify uptime is increasing
- ✅ Look for errors in Serial Monitor

### Button Issues

**Navigation Not Working:**
- ✅ BOOT button is built-in on most ESP32 boards
- ✅ Connected to GPIO0 internally
- ✅ Wait 200ms between presses (debounce)
- ✅ Button should be pressed briefly, not held

### Compilation Errors

**Library Not Found:**
```
fatal error: Adafruit_ST7735.h: No such file or directory
```
- ✅ Install all required libraries via Library Manager
- ✅ Restart Arduino IDE after installation

**ESP32 Board Not Found:**
```
Error: board 'esp32' not found
```
- ✅ Add ESP32 board manager URL
- ✅ Install ESP32 board package
- ✅ Select correct board in Tools menu

## 📝 Configuration

### Update Intervals

```cpp
// Sensor reading interval (milliseconds)
if (millis() - lastRead >= 5000) {  // 5 seconds
    // Change to 10000 for 10 seconds
    // Change to 60000 for 1 minute
}
```

### Data Logging

```cpp
#define MAX_LOG 288  // 288 points = 24 hours at 5-minute intervals
// Adjust based on memory and desired history length
// 144 = 12 hours at 5-minute intervals
// 576 = 48 hours at 5-minute intervals
```

### Display Orientation

```cpp
tft.setRotation(3);  // Landscape
// 0 = Portrait
// 1 = Landscape (rotated right)
// 2 = Portrait (upside down)
// 3 = Landscape (rotated left)
```

### Color Theme

Customize colors in the code:
```cpp
#define BG        0x0000  // Background
#define TEXT      0xFFFF  // Primary text
#define TEXT_DIM  0x7BEF  // Secondary text
#define ACCENT    0x07FF  // Accent color (cyan)
#define SUCCESS   0x07E0  // Success (green)
#define WARNING   0xFD20  // Warning (orange)
#define DANGER    0xF800  // Danger (red)
```

### WiFi Settings

```cpp
const char* WIFI_SSID = "YourNetworkName";
const char* WIFI_PASSWORD = "YourPassword";
const int HTTP_PORT = 80;  // Change if port 80 is blocked
```

## 🐛 Known Issues

- **Memory**: Large log arrays (288 points) may cause issues on heavily loaded systems
- **WiFi**: Connection can take 10-20 seconds on slow networks
- **Display**: Some ST7735 clones may need different initialization modes

## 📚 Technical Details

### Memory Usage
- Flash: ~1.2 MB (code + libraries)
- SRAM: ~40 KB (including 288-point data log)
- Available: ESP32 has 4MB Flash, 520KB SRAM

### Power Consumption
- Active (WiFi on): ~160 mA
- Display backlight: ~20 mA
- Sensors: ~5 mA combined
- **Total**: ~185 mA @ 3.3V

### Performance
- Sensor readings: Every 5 seconds
- Display updates: Real-time when data changes
- HTTP requests: Handled instantly
- WiFi stability: Auto-reconnect on disconnect

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.


## 👤 Author

**Marautech**

- GitHub: [@Marautech](https://github.com/Marautech)
- Project: Weather Station ESP32 v2.0

---

**Made with ❤️ by Marautech**

*Weather Station v2.0 - Professional ESP32 IoT Solution*
