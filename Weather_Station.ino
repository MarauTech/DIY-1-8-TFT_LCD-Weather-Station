/*
 * Simple Weather Station ESP32 with JSON API
 * Display: TFT 1.8" ST7735
 * Sensors: 2x DS18B20, BMP280+AHT20
 * Author: Marautech
 * Version: 2.0 - Clean + JSON
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    2
#define ONE_WIRE_BUS 15
#define I2C_SDA 21
#define I2C_SCL 22
#define BUTTON_PIN 0
#define LED_PIN 26

const char* WIFI_SSID = "sterowanie";
const char* WIFI_PASSWORD = "Adam1998";
const int HTTP_PORT = 80;

#define BG        0x0000  // Black
#define TEXT      0xFFFF  // White
#define TEXT_DIM  0x7BEF  // Gray
#define ACCENT    0x07FF  // Cyan
#define SUCCESS   0x07E0  // Green
#define WARNING   0xFD20  // Orange
#define DANGER    0xF800  // Red

#define MAX_LOG 288
struct DataLog {
  float tempIn, tempOut, humidity, pressure;
};
DataLog logs[MAX_LOG];
int logIdx = 0;
int logCnt = 0;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds(&oneWire);
Adafruit_BMP280 bmp;
Adafruit_AHTX0 aht;
WebServer server(HTTP_PORT);

DeviceAddress sensor1, sensor2;


bool ds18b20OK = false;
bool bmp280OK = false;
bool aht20OK = false;
bool wifiOK = false;

struct {
  float tempIn = 0;
  float tempOut = 0;
  float humidity = 0;
  float pressure = 0;
  float dewPoint = 0;
} data;

byte screen = 0;
byte maxScreens = 3;
bool btnLast = HIGH;
unsigned long lastBtn = 0;
unsigned long lastRead = 0;
unsigned long startTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Weather Station v2.0 ===");
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  startTime = millis();
  
  initDisplay();
  showBoot("Starting...");
  delay(500);
  
  Wire.begin(I2C_SDA, I2C_SCL);
  
  showBoot("Init BMP280...");
  bmp280OK = bmp.begin(0x76) || bmp.begin(0x77);
  if (bmp280OK) {
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);
  }
  delay(300);
  
  showBoot("Init AHT20...");
  aht20OK = aht.begin();
  delay(300);
  
  showBoot("Init DS18B20...");
  ds.begin();
  if (ds.getDeviceCount() >= 2) {
    ds.getAddress(sensor1, 0);
    ds.getAddress(sensor2, 1);
    ds18b20OK = true;
  }
  delay(300);
  
  showBoot("Connecting WiFi...");
  connectWiFi();
  
  if (wifiOK) {
    showBoot("Starting HTTP...");
    setupWebServer();
    server.begin();
    
    tft.fillScreen(BG);
    tft.setTextSize(1);
    tft.setTextColor(SUCCESS);
    tft.setCursor(5, 40);
    tft.print("WiFi Connected!");
    
    tft.setTextColor(TEXT);
    tft.setCursor(5, 55);
    tft.print("IP Address:");
    
    tft.setTextSize(2);
    tft.setTextColor(ACCENT);
    tft.setCursor(5, 70);
    tft.print(WiFi.localIP());
    
    tft.setTextSize(1);
    tft.setTextColor(TEXT_DIM);
    tft.setCursor(5, 95);
    tft.print("JSON: /data");
    tft.setCursor(5, 107);
    tft.print("WEB: /");
    
    Serial.println("=== SYSTEM READY ===");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("JSON API: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/data");
    Serial.print("Web Dashboard: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
    
    delay(5000);
  } else {
    showBoot("WiFi Failed!");
    Serial.println("WiFi connection failed!");
    delay(2000);
  }
  
  drawScreen();
}

void loop() {
  handleButton();
  
  if (wifiOK) server.handleClient();
  
  if (millis() - lastRead >= 5000) {
    lastRead = millis();
    readSensors();
    updateLog();
    updateDisplay();
  }
}

void initDisplay() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(BG);
}

void showBoot(const char* msg) {
  tft.fillScreen(BG);
  
  tft.setTextSize(3);
  tft.setTextColor(ACCENT);
  tft.setCursor(20, 25);
  tft.print("WEATHER");
  
  tft.setTextSize(1);
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(50, 55);
  tft.print("STATION v2.0");
  
  tft.setTextSize(1);
  tft.setTextColor(TEXT);
  int msgX = (160 - strlen(msg) * 6) / 2;
  tft.setCursor(msgX, 75);
  tft.print(msg);
  
  int y = 100;
  tft.fillCircle(50, y, 3, ds18b20OK ? SUCCESS : TEXT_DIM);
  tft.fillCircle(70, y, 3, bmp280OK ? SUCCESS : TEXT_DIM);
  tft.fillCircle(90, y, 3, aht20OK ? SUCCESS : TEXT_DIM);
  tft.fillCircle(110, y, 3, wifiOK ? SUCCESS : TEXT_DIM);
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(300);
    tries++;
  }
  wifiOK = (WiFi.status() == WL_CONNECTED);
}

void setupWebServer() {
  server.on("/data", HTTP_GET, []() {
    StaticJsonDocument<512> doc;
    
    doc["station"] = "WeatherStation";
    doc["version"] = "2.0";
    doc["uptime"] = (millis() - startTime) / 1000;
    
    JsonObject temp = doc.createNestedObject("temperature");
    temp["indoor"] = roundf(data.tempIn * 10) / 10;
    temp["outdoor"] = roundf(data.tempOut * 10) / 10;
    temp["unit"] = "celsius";
    
    JsonObject env = doc.createNestedObject("environment");
    env["humidity"] = roundf(data.humidity * 10) / 10;
    env["pressure"] = roundf(data.pressure * 10) / 10;
    env["dewPoint"] = roundf(data.dewPoint * 10) / 10;
    
    JsonObject sensors = doc.createNestedObject("sensors");
    sensors["ds18b20"] = ds18b20OK;
    sensors["bmp280"] = bmp280OK;
    sensors["aht20"] = aht20OK;
    
    JsonObject network = doc.createNestedObject("network");
    network["wifi"] = wifiOK;
    network["ip"] = WiFi.localIP().toString();
    network["rssi"] = WiFi.RSSI();
    
    JsonObject stats = doc.createNestedObject("statistics");
    stats["dataPoints"] = logCnt;
    stats["maxPoints"] = MAX_LOG;
    
    String output;
    serializeJsonPretty(doc, output);
    
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", output);
  });
  
  server.on("/", HTTP_GET, []() {
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>Weather Station v2.0</title>";
    html += "<style>";
    html += "body{font-family:Arial;background:#111;color:#fff;padding:20px;margin:0}";
    html += ".card{background:#222;border-radius:10px;padding:20px;margin:10px 0}";
    html += "h1{color:#00ffff;margin:0 0 20px 0}";
    html += ".value{font-size:2em;color:#00ffff;margin:10px 0}";
    html += ".label{color:#999;font-size:0.9em}";
    html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:15px}";
    html += "</style>";
    html += "<script>";
    html += "function update(){";
    html += "fetch('/data').then(r=>r.json()).then(d=>{";
    html += "document.getElementById('tempIn').innerText=d.temperature.indoor+'°C';";
    html += "document.getElementById('tempOut').innerText=d.temperature.outdoor+'°C';";
    html += "document.getElementById('hum').innerText=d.environment.humidity+'%';";
    html += "document.getElementById('press').innerText=d.environment.pressure+' hPa';";
    html += "document.getElementById('dew').innerText=d.environment.dewPoint+'°C';";
    html += "document.getElementById('ip').innerText=d.network.ip;";
    html += "document.getElementById('rssi').innerText=d.network.rssi+' dBm';";
    html += "document.getElementById('uptime').innerText=Math.floor(d.uptime/60)+' min';";
    html += "});}";
    html += "setInterval(update,5000);";
    html += "update();";
    html += "</script></head><body>";
    html += "<h1>🌡️ Weather Station v2.0</h1>";
    html += "<div class='grid'>";
    html += "<div class='card'><div class='label'>Indoor Temperature</div><div class='value' id='tempIn'>--</div></div>";
    html += "<div class='card'><div class='label'>Outdoor Temperature</div><div class='value' id='tempOut'>--</div></div>";
    html += "<div class='card'><div class='label'>Humidity</div><div class='value' id='hum'>--</div></div>";
    html += "<div class='card'><div class='label'>Pressure</div><div class='value' id='press'>--</div></div>";
    html += "<div class='card'><div class='label'>Dew Point</div><div class='value' id='dew'>--</div></div>";
    html += "</div>";
    html += "<div class='card'>";
    html += "<div class='label'>System Info</div>";
    html += "<div style='margin-top:10px'><span class='label'>IP:</span> <span id='ip'>--</span></div>";
    html += "<div><span class='label'>Signal:</span> <span id='rssi'>--</span></div>";
    html += "<div><span class='label'>Uptime:</span> <span id='uptime'>--</span></div>";
    html += "</div>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
  });
}

void readSensors() {
  if (ds18b20OK) {
    ds.requestTemperatures();
    data.tempIn = ds.getTempC(sensor1);
    data.tempOut = ds.getTempC(sensor2);
  }
  
  if (bmp280OK) {
    data.pressure = bmp.readPressure() / 100.0F;
  }
  
  if (aht20OK) {
    sensors_event_t h, t;
    aht.getEvent(&h, &t);
    data.humidity = h.relative_humidity;
  }
  
  if (data.humidity > 0) {
    float a = 17.27;
    float b = 237.7;
    float temp = bmp280OK ? bmp.readTemperature() : data.tempIn;
    float alpha = ((a * temp) / (b + temp)) + log(data.humidity / 100.0);
    data.dewPoint = (b * alpha) / (a - alpha);
  }
}

void updateLog() {
  logs[logIdx].tempIn = data.tempIn;
  logs[logIdx].tempOut = data.tempOut;
  logs[logIdx].humidity = data.humidity;
  logs[logIdx].pressure = data.pressure;
  
  logIdx = (logIdx + 1) % MAX_LOG;
  if (logCnt < MAX_LOG) logCnt++;
}

void handleButton() {
  bool btn = digitalRead(BUTTON_PIN);
  if (btn == LOW && btnLast == HIGH && millis() - lastBtn > 200) {
    lastBtn = millis();
    screen = (screen + 1) % maxScreens;
    drawScreen();
  }
  btnLast = btn;
}

void drawScreen() {
  tft.fillScreen(BG);
  
  switch(screen) {
    case 0: drawMainScreen(); break;
    case 1: drawGraphScreen(); break;
    case 2: drawInfoScreen(); break;
  }
}

void updateDisplay() {
  switch(screen) {
    case 0: updateMainScreen(); break;
    case 1: drawGraphScreen(); break;
  }
}

void drawMainScreen() {
  tft.setTextSize(1);
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(5, 5);
  tft.print("WEATHER STATION v2.0");
  
  tft.fillCircle(152, 8, 2, wifiOK ? SUCCESS : TEXT_DIM);
  
  tft.drawFastHLine(0, 16, 160, TEXT_DIM);
  
  drawTempValue(10, 25, "INDOOR", data.tempIn);
  drawTempValue(10, 60, "OUTDOOR", data.tempOut);
  
  tft.drawFastHLine(0, 95, 160, TEXT_DIM);
  
  tft.setTextSize(1);
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(5, 102);
  tft.print("HUM");
  tft.setCursor(5, 114);
  tft.print("BAR");
  
  tft.setCursor(85, 102);
  tft.print("DEW");
  tft.setCursor(85, 114);
  tft.print("LOG");
}

void drawTempValue(int x, int y, const char* label, float temp) {
  tft.setTextSize(1);
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(x, y);
  tft.print(label);
  
  tft.setTextSize(3);
  tft.setTextColor(getTempColor(temp));
  tft.setCursor(x, y + 10);
  tft.print(temp, 1);
  
  tft.setTextSize(1);
  tft.print("C");
}

void updateMainScreen() {
  tft.fillRect(10, 35, 140, 24, BG);
  tft.setTextSize(3);
  tft.setTextColor(getTempColor(data.tempIn));
  tft.setCursor(10, 35);
  tft.print(data.tempIn, 1);
  tft.setTextSize(1);
  tft.print("C");
  
  tft.fillRect(10, 70, 140, 24, BG);
  tft.setTextSize(3);
  tft.setTextColor(getTempColor(data.tempOut));
  tft.setCursor(10, 70);
  tft.print(data.tempOut, 1);
  tft.setTextSize(1);
  tft.print("C");
  
  tft.fillRect(30, 100, 50, 24, BG);
  tft.fillRect(110, 100, 50, 24, BG);
  
  tft.setTextSize(1);
  tft.setTextColor(ACCENT);
  tft.setCursor(30, 102);
  tft.print(data.humidity, 0);
  tft.print("%");
  
  tft.setCursor(30, 114);
  tft.print(data.pressure, 0);
  
  tft.setTextColor(WARNING);
  tft.setCursor(110, 102);
  tft.print(data.dewPoint, 1);
  
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(110, 114);
  tft.print(logCnt);
}

void drawGraphScreen() {
  tft.setTextSize(1);
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(5, 5);
  tft.print("TRENDS");
  tft.drawFastHLine(0, 16, 160, TEXT_DIM);
  
  if (logCnt < 2) {
    tft.setTextColor(TEXT);
    tft.setCursor(20, 60);
    tft.print("Collecting data...");
    return;
  }
  
  drawGraph(5, 25, 150, 50, WARNING);
  
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(5, 82);
  tft.print("Temperature (24h)");
  
  drawGraph(5, 95, 150, 25, ACCENT);
  
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(5, 122);
  tft.print("Pressure");
}

void drawGraph(int x, int y, int w, int h, uint16_t color) {
  tft.drawRect(x, y, w, h, TEXT_DIM);
  
  float minVal = 999999, maxVal = -999999;
  bool isTemp = (color == WARNING);
  
  for (int i = 0; i < logCnt; i++) {
    float val = isTemp ? logs[i].tempIn : logs[i].pressure;
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
  }
  
  if (maxVal - minVal < 0.1) return;
  
  int points = min(logCnt, w - 4);
  for (int i = 1; i < points; i++) {
    int idx1 = (logIdx - points + i - 1 + MAX_LOG) % MAX_LOG;
    int idx2 = (logIdx - points + i + MAX_LOG) % MAX_LOG;
    
    float val1 = isTemp ? logs[idx1].tempIn : logs[idx1].pressure;
    float val2 = isTemp ? logs[idx2].tempIn : logs[idx2].pressure;
    
    int y1 = y + h - 2 - ((val1 - minVal) / (maxVal - minVal)) * (h - 4);
    int y2 = y + h - 2 - ((val2 - minVal) / (maxVal - minVal)) * (h - 4);
    
    tft.drawLine(x + i, y1, x + i + 1, y2, color);
  }
}

void drawInfoScreen() {
  tft.setTextSize(1);
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(5, 5);
  tft.print("SYSTEM INFO");
  tft.drawFastHLine(0, 16, 160, TEXT_DIM);
  
  int y = 25;
  

  tft.setTextColor(TEXT);
  tft.setCursor(5, y);
  tft.print("DS18B20:");
  tft.setTextColor(ds18b20OK ? SUCCESS : DANGER);
  tft.setCursor(80, y);
  tft.print(ds18b20OK ? "OK" : "ERROR");
  
  y += 15;
  tft.setTextColor(TEXT);
  tft.setCursor(5, y);
  tft.print("BMP280:");
  tft.setTextColor(bmp280OK ? SUCCESS : DANGER);
  tft.setCursor(80, y);
  tft.print(bmp280OK ? "OK" : "ERROR");
  
  y += 15;
  tft.setTextColor(TEXT);
  tft.setCursor(5, y);
  tft.print("AHT20:");
  tft.setTextColor(aht20OK ? SUCCESS : DANGER);
  tft.setCursor(80, y);
  tft.print(aht20OK ? "OK" : "ERROR");
  
  y += 20;
  tft.drawFastHLine(0, y, 160, TEXT_DIM);
  y += 8;
  
  tft.setTextColor(TEXT);
  tft.setCursor(5, y);
  tft.print("WiFi:");
  tft.setTextColor(wifiOK ? SUCCESS : DANGER);
  tft.setCursor(80, y);
  tft.print(wifiOK ? "OK" : "OFF");
  
  if (wifiOK) {
    y += 15;
    tft.setTextColor(TEXT_DIM);
    tft.setCursor(5, y);
    tft.print("IP Address:");
    y += 10;
    tft.setTextColor(ACCENT);
    tft.setCursor(5, y);
    tft.print(WiFi.localIP());
    
    y += 15;
    tft.setTextColor(TEXT_DIM);
    tft.setCursor(5, y);
    tft.print("JSON: /data");
  }
  
  tft.setTextColor(TEXT_DIM);
  tft.setCursor(60, 118);
  tft.print("v2.0 JSON");
}


uint16_t getTempColor(float temp) {
  if (temp < 15) return ACCENT;
  if (temp < 22) return SUCCESS;
  if (temp < 26) return WARNING;
  return DANGER;
}
