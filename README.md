# 🌤️ Weather Station ESP32

Professional weather station based on ESP32 with TFT display and multiple sensors.

![Version](https://img.shields.io/badge/version-3.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Author](https://img.shields.io/badge/author-Marautech-purple)

## 📸 Previev
![6046104065235684266](https://github.com/user-attachments/assets/858dc8a5-6725-4863-bff7-e645507594e9)


## ✨ Features

- **Multi-sensor support**: DS18B20 (temperature), AHT20 (temperature & humidity), BMP280 (pressure)
- **TFT Display**: 1.8" ST7735 color display with landscape orientation
- **3 Screen Navigation**: Switch between screens using BOOT button
- **Color-coded Values**: Visual feedback with color temperature, humidity, and pressure indicators
- **Real-time Updates**: Data refreshes every 3 seconds
- **Professional UI**: Clean, modern interface optimized for horizontal display

## 🔧 Hardware

### Main Components

- **Microcontroller**: ESP32 DevKit v1
- **Display**: TFT 1.8" LCD ST7735 (SPI)
- **Temperature Sensors**: 2x DS18B20 (1-Wire)
- **Environment Sensor**: AHT20 (I2C)
- **Pressure Sensor**: BMP280 (I2C)

### Requirements

- ESP32 Development Board
- ST7735 TFT Display Module
- 2x DS18B20 Temperature Sensors
- AHT20 Temperature & Humidity Sensor
- BMP280 Barometric Pressure Sensor
- Resistors: 1x 4.7kΩ (pull-up for DS18B20)
- Breadboard and jumper wires

## 📊 Sensors

### DS18B20 Temperature Sensors
- **Interface**: 1-Wire
- **Range**: -55°C to +125°C
- **Accuracy**: ±0.5°C
- **Usage**: Inside and outside temperature measurement

### AHT20 Temperature & Humidity Sensor
- **Interface**: I2C
- **Temperature Range**: -40°C to +85°C
- **Humidity Range**: 0% to 100% RH
- **Accuracy**: ±2% RH

### BMP280 Barometric Pressure Sensor
- **Interface**: I2C
- **Pressure Range**: 300 to 1100 hPa
- **Accuracy**: ±1 hPa
- **Temperature Range**: -40°C to +85°C

## 📡 Pinout

| Component | Pin | ESP32 GPIO | Notes |
|-----------|-----|------------|-------|
| **TFT ST7735** |
| VCC | - | 3.3V | Power |
| GND | - | GND | Ground |
| CS | - | GPIO5 | Chip Select |
| RESET | - | GPIO4 | Reset |
| DC/A0 | - | GPIO2 | Data/Command |
| SDA/MOSI | - | GPIO23 | SPI Data |
| SCK | - | GPIO18 | SPI Clock |
| LED | - | 3.3V | Backlight |
| **DS18B20** |
| DATA | - | GPIO15 | 1-Wire Data |
| VCC | - | 3.3V | Power |
| GND | - | GND | Ground |
| Pull-up | - | 4.7kΩ | Between DATA and 3.3V |
| **AHT20** |
| VCC | - | 3.3V | Power |
| GND | - | GND | Ground |
| SDA | - | GPIO21 | I2C Data |
| SCL | - | GPIO22 | I2C Clock |
| **BMP280** |
| VCC | - | 3.3V | Power |
| GND | - | GND | Ground |
| SDA | - | GPIO21 | I2C Data (shared with AHT20) |
| SCL | - | GPIO22 | I2C Clock (shared with AHT20) |
| **Controls** |
| BOOT Button | - | GPIO0 | Screen navigation |

## 🚀 Installation

### 1. Install Arduino IDE

Download and install [Arduino IDE](https://www.arduino.cc/en/software) version 1.8.x or later.

### 2. Add ESP32 Board Support

1. Open Arduino IDE
2. Go to **File** → **Preferences**
3. Add this URL to **Additional Boards Manager URLs**:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools** → **Board** → **Boards Manager**
5. Search for "ESP32" and install it
6. Select **Tools** → **Board** → **ESP32 Arduino** → **ESP32 Dev Module**

### 3. Install Required Libraries

In Arduino IDE, go to **Sketch** → **Include Library** → **Manage Libraries** and install:

- **Adafruit GFX Library** (by Adafruit)
- **Adafruit ST7735 and ST7789 Library** (by Adafruit)
- **OneWire** (by Paul Stoffregen)
- **DallasTemperature** (by Miles Burton)
- **DFRobot AHT20** (by DFRobot)
- **Adafruit BMP280 Library** (by Adafruit)

### 4. Upload Code

1. Clone this repository:

2. Open `Stacja_Pogodowa.ino` in Arduino IDE

3. Connect ESP32 to your computer via USB

4. Select the correct COM port: **Tools** → **Port**

5. Click **Upload** button

6. Wait for compilation and upload to complete

## 🎮 Usage

### Power On

After uploading the code, the weather station will automatically:
1. Display welcome screen for 2 seconds
2. Initialize all sensors
3. Show Screen 1 (Temperature)

### Navigation

Press the **BOOT button** (GPIO0) on ESP32 to switch between screens:
- **Screen 1**: Inside & Outside Temperature
- **Screen 2**: Humidity & Pressure
- **Screen 3**: About & Sensor Status

### Data Updates

All sensor data refreshes automatically every **3 seconds**.

### Serial Monitor

Open Serial Monitor (115200 baud) to view sensor readings and debug information:
```
=== Weather Station ESP32 ===
Initializing TFT display...
Initializing I2C bus...
Initializing AHT20 sensor...
AHT20 detected!
Initializing BMP280 sensor...
BMP280 detected at address 0x76!
Detecting DS18B20 sensors...
Found devices: 2
Sensors detected successfully!
Weather station ready!

=== SENSOR READINGS ===
DS18B20 In: 23.50 °C
DS18B20 Out: 18.20 °C
AHT20 Temp: 23.10 °C
AHT20 Hum: 45.30 %
BMP280 Press: 1015.20 hPa
-------------------
```

## 📚 Libraries

This project uses the following libraries:

- **Adafruit_GFX**: Graphics library for displays
- **Adafruit_ST7735**: ST7735 TFT display driver
- **OneWire**: 1-Wire communication protocol
- **DallasTemperature**: DS18B20 temperature sensor driver
- **DFRobot_AHT20**: AHT20 sensor driver
- **Adafruit_BMP280**: BMP280 sensor driver
- **Wire**: I2C communication
- **SPI**: SPI communication

## 🖥️ Screens

### Screen 1: Temperature
Displays inside and outside temperatures from DS18B20 sensors.

**Color Coding:**
- 🔵 Blue: Cold (< 20°C inside, < 10°C outside)
- 🟢 Green: Normal (20-25°C inside, 10-20°C outside)
- 🔴 Red: Warm (> 25°C inside, > 20°C outside)

### Screen 2: Humidity & Pressure
Shows humidity from AHT20 and pressure from BMP280.

**Color Coding:**
- Humidity:
  - 🔵 Blue: Dry (< 30%)
  - 🟢 Green: Normal (30-70%)
  - 🔴 Red: Humid (> 70%)
- Pressure:
  - 🔵 Blue: Low (< 980 hPa)
  - ⚪ White: Normal (980-1020 hPa)
  - 🔴 Red: High (> 1020 hPa)

### Screen 3: About
Displays author information, sensor status, and firmware version.

**Status Indicators:**
- 🟢 Green "OK": Sensor detected and working
- 🔴 Red "NO": Sensor not detected

## 🔧 Troubleshooting

### Display Shows Glitches

If you see colored bars on the edges:
1. Try changing the initialization in code:
   ```cpp
   // Change from:
   tft.initR(INITR_BLACKTAB);
   // To:
   tft.initR(INITR_GREENTAB);
   ```

### Sensors Not Detected

**DS18B20:**
- Check if pull-up resistor (4.7kΩ) is connected between DATA and 3.3V
- Verify DATA pin is connected to GPIO15
- Check wiring continuity

**AHT20/BMP280:**
- Verify I2C connections (SDA → GPIO21, SCL → GPIO22)
- Check if both sensors share the same I2C bus properly
- BMP280 might be on 0x77 instead of 0x76 (code handles both automatically)

### Button Not Working

- Ensure BOOT button is connected to GPIO0
- Button requires pull-up (built-in on ESP32)
- Wait 200ms between presses (debounce time)

### Compilation Errors

- Verify all libraries are installed correctly
- Check ESP32 board is selected in Arduino IDE
- Update libraries to latest versions
- Check internet connection for library downloads

## 📝 Configuration

### Update Interval

Change the sensor reading interval in code:
```cpp
const unsigned long updateInterval = 3000;  // milliseconds
```

### Display Orientation

Change screen rotation:
```cpp
tft.setRotation(3);  // 0-3 for different orientations
```

### Color Thresholds

Adjust temperature thresholds:
```cpp
uint16_t colorInside = (tempInside < 20) ? BLUE : (tempInside > 25) ? RED : GREEN;
```


## 👤 Author

**Marautech**

- GitHub: [@Marautech](https://github.com/Marautech)



**Made with ❤️ by Marautech**

