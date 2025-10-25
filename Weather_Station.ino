/*
 * Weather Station ESP32
 * Display: TFT 1.8" ST7735
 * Sensors: 2x DS18B20, AHT20, BMP280
 * Author: Marautech
 * Version: 3.0
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DFRobot_AHT20.h>
#include <Adafruit_BMP280.h>

#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    2
#define TFT_SCLK  18
#define TFT_MOSI  23

#define ONE_WIRE_BUS 15

#define I2C_SDA 21
#define I2C_SCL 22

#define BUTTON_PIN 0

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DFRobot_AHT20 aht20;
Adafruit_BMP280 bmp280;

DeviceAddress insideThermometer;
DeviceAddress outsideThermometer;
bool sensorsDetected = false;
bool aht20Detected = false;
bool bmp280Detected = false;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 3000;

byte currentScreen = 0;
bool lastButtonState = true;
unsigned long lastButtonPress = 0;

#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define WHITE     0xFFFF

void setup() {
  Serial.begin(115200);
  Serial.println("=== Weather Station ESP32 ===");
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Initializing TFT display...");
  
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(10);
  digitalWrite(TFT_RST, HIGH);
  delay(10);
  
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  
  tft.fillScreen(BLACK);
  
  showWelcomeScreen();
  delay(2000);
  
  Serial.println("Initializing I2C bus...");
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(100);
  
  Serial.println("Initializing AHT20 sensor...");
  if (aht20.begin()) {
    aht20Detected = true;
    Serial.println("AHT20 detected!");
  } else {
    aht20Detected = false;
    Serial.println("ERROR: AHT20 not detected");
  }
  
  Serial.println("Initializing BMP280 sensor...");
  if (bmp280.begin(0x76)) {
    bmp280Detected = true;
    Serial.println("BMP280 detected at address 0x76!");
  } else if (bmp280.begin(0x77)) {
    bmp280Detected = true;
    Serial.println("BMP280 detected at address 0x77!");
  } else {
    bmp280Detected = false;
    Serial.println("ERROR: BMP280 not detected");
  }
  
  Serial.println("Initializing DS18B20 sensors...");
  sensors.begin();
  
  detectSensors();
  
  tft.fillScreen(BLACK);
  drawCurrentScreen();
  
  Serial.println("Weather station ready!");
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH && (millis() - lastButtonPress > 200)) {
    lastButtonPress = millis();
    currentScreen = (currentScreen + 1) % 3;
    drawCurrentScreen();
    Serial.print("Switched to screen: ");
    Serial.println(currentScreen);
  }
  lastButtonState = currentButtonState;
  
  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();
    
    float tempInside = -999;
    float tempOutside = -999;
    float tempAHT20 = -999;
    float humidity = -999;
    float pressure = -999;
    
    if (sensorsDetected) {
      sensors.requestTemperatures();
      tempInside = sensors.getTempC(insideThermometer);
      tempOutside = sensors.getTempC(outsideThermometer);
    } else {
      detectSensors();
    }
    
    if (aht20Detected) {
      if (aht20.startMeasurementReady(true)) {
        tempAHT20 = aht20.getTemperature_C();
        humidity = aht20.getHumidity_RH();
      }
    }
    
    if (bmp280Detected) {
      pressure = bmp280.readPressure() / 100.0;
    }
    
    displayCurrentScreen(tempInside, tempOutside, tempAHT20, humidity, pressure);
    
    Serial.println("=== SENSOR READINGS ===");
    if (sensorsDetected) {
      Serial.print("DS18B20 In: ");
      Serial.print(tempInside);
      Serial.println(" °C");
      Serial.print("DS18B20 Out: ");
      Serial.print(tempOutside);
      Serial.println(" °C");
    }
    if (aht20Detected) {
      Serial.print("AHT20 Temp: ");
      Serial.print(tempAHT20);
      Serial.println(" °C");
      Serial.print("AHT20 Hum: ");
      Serial.print(humidity);
      Serial.println(" %");
    }
    if (bmp280Detected) {
      Serial.print("BMP280 Press: ");
      Serial.print(pressure);
      Serial.println(" hPa");
    }
    Serial.println("-------------------");
  }
}

void showWelcomeScreen() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  
  tft.setCursor(25, 50);
  tft.setTextSize(2);
  tft.print("Weather");
  
  tft.setCursor(30, 75);
  tft.print("Station");
  
  tft.setCursor(35, 100);
  tft.setTextSize(1);
  tft.print("ESP32");
  
  tft.drawFastHLine(10, 118, 140, CYAN);
}

void detectSensors() {
  Serial.println("Detecting DS18B20 sensors...");
  
  int deviceCount = sensors.getDeviceCount();
  Serial.print("Found devices: ");
  Serial.println(deviceCount);
  
  if (deviceCount >= 2) {
    DeviceAddress tempDeviceAddress;
    
    for (int i = 0; i < deviceCount; i++) {
      if (sensors.getAddress(tempDeviceAddress, i)) {
        Serial.print("Sensor ");
        Serial.print(i + 1);
        Serial.print(": ");
        printAddress(tempDeviceAddress);
        Serial.println();
        
        if (i == 0) {
          copyAddress(tempDeviceAddress, insideThermometer);
        } else if (i == 1) {
          copyAddress(tempDeviceAddress, outsideThermometer);
        }
      }
    }
    
    sensorsDetected = true;
    Serial.println("Sensors detected successfully!");
  } else {
    sensorsDetected = false;
    Serial.println("ERROR: Required number of sensors not detected (required: 2)");
    
    tft.fillScreen(BLACK);
    tft.setTextColor(RED);
    tft.setTextSize(1);
    tft.setCursor(10, 50);
    tft.print("ERROR:");
    tft.setCursor(10, 70);
    tft.print("Sensors not");
    tft.setCursor(10, 90);
    tft.print("detected!");
    tft.setCursor(10, 110);
    tft.print("Found: ");
    tft.print(deviceCount);
  }
}

void drawCurrentScreen() {
  tft.fillScreen(BLACK);
  
  switch(currentScreen) {
    case 0:
      drawScreen0_Temperature();
      break;
    case 1:
      drawScreen1_HumidityPressure();
      break;
    case 2:
      drawScreen2_About();
      break;
  }
}

void displayCurrentScreen(float tempInside, float tempOutside, float tempAHT20, float humidity, float pressure) {
  switch(currentScreen) {
    case 0:
      displayScreen0_Temperature(tempInside, tempOutside);
      break;
    case 1:
      displayScreen1_HumidityPressure(humidity, pressure);
      break;
    case 2:
      break;
  }
}

void drawScreen0_Temperature() {
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.setCursor(50, 5);
  tft.print("TEMPERATURE");
  
  tft.drawFastHLine(0, 15, 159, CYAN);
  
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  
  tft.setCursor(10, 30);
  tft.print("Inside:");
  
  tft.setCursor(10, 70);
  tft.print("Outside:");
}

void displayScreen0_Temperature(float tempInside, float tempOutside) {
  tft.setTextSize(2);
  
  tft.fillRect(70, 22, 85, 20, BLACK);
  if (tempInside != -999 && tempInside != DEVICE_DISCONNECTED_C) {
    uint16_t colorInside = (tempInside < 20) ? BLUE : (tempInside > 25) ? RED : GREEN;
    tft.setTextColor(colorInside);
    tft.setCursor(70, 22);
    tft.print(tempInside, 1);
    tft.setTextSize(1);
    tft.print("C");
  } else {
    tft.setTextColor(RED);
    tft.setCursor(70, 22);
    tft.print("---");
  }
  
  tft.setTextSize(2);
  tft.fillRect(70, 62, 85, 20, BLACK);
  if (tempOutside != -999 && tempOutside != DEVICE_DISCONNECTED_C) {
    uint16_t colorOutside = (tempOutside < 10) ? BLUE : (tempOutside > 20) ? RED : GREEN;
    tft.setTextColor(colorOutside);
    tft.setCursor(70, 62);
    tft.print(tempOutside, 1);
    tft.setTextSize(1);
    tft.print("C");
  } else {
    tft.setTextColor(RED);
    tft.setCursor(70, 62);
    tft.print("---");
  }
}

void drawScreen1_HumidityPressure() {
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.setCursor(15, 5);
  tft.print("HUMIDITY & PRESSURE");
  
  tft.drawFastHLine(0, 15, 159, CYAN);
  
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  
  tft.setCursor(10, 30);
  tft.print("Humidity:");
  
  tft.setCursor(10, 70);
  tft.print("Pressure:");
}

void displayScreen1_HumidityPressure(float humidity, float pressure) {
  tft.setTextSize(2);
  
  tft.fillRect(70, 22, 85, 20, BLACK);
  if (humidity != -999) {
    uint16_t colorHum = (humidity < 30) ? BLUE : (humidity > 70) ? RED : GREEN;
    tft.setTextColor(colorHum);
    tft.setCursor(70, 22);
    tft.print(humidity, 1);
    tft.setTextSize(1);
    tft.print("%");
  } else {
    tft.setTextColor(RED);
    tft.setCursor(70, 22);
    tft.print("---");
  }
  
  tft.setTextSize(2);
  tft.fillRect(70, 62, 85, 20, BLACK);
  if (pressure != -999) {
    uint16_t colorPress = WHITE;
    if (pressure < 980) colorPress = BLUE;
    else if (pressure > 1020) colorPress = RED;
    tft.setTextColor(colorPress);
    tft.setCursor(70, 62);
    tft.print(pressure, 0);
    tft.setTextSize(1);
    tft.print("hPa");
  } else {
    tft.setTextColor(RED);
    tft.setCursor(70, 62);
    tft.print("---");
  }
}

void drawScreen2_About() {
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.setCursor(60, 5);
  tft.print("ABOUT");
  
  tft.drawFastHLine(0, 15, 159, CYAN);
  
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.setCursor(45, 30);
  tft.print("Marautech");
  
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  
  tft.setCursor(5, 50);
  tft.print("DS18B20:");
  tft.setTextColor(sensorsDetected ? GREEN : RED);
  tft.setCursor(80, 50);
  tft.print(sensorsDetected ? "OK" : "NO");
  
  tft.setTextColor(WHITE);
  tft.setCursor(5, 70);
  tft.print("AHT20:");
  tft.setTextColor(aht20Detected ? GREEN : RED);
  tft.setCursor(80, 70);
  tft.print(aht20Detected ? "OK" : "NO");
  
  tft.setTextColor(WHITE);
  tft.setCursor(5, 90);
  tft.print("BMP280:");
  tft.setTextColor(bmp280Detected ? GREEN : RED);
  tft.setCursor(80, 90);
  tft.print(bmp280Detected ? "OK" : "NO");
  
  tft.setTextColor(CYAN);
  tft.setCursor(55, 110);
  tft.print("Ver: 3.0");
}

void copyAddress(DeviceAddress from, DeviceAddress to) {
  for (uint8_t i = 0; i < 8; i++) {
    to[i] = from[i];
  }
}

void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
