#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <DHT.h>

#define BUZZER 33
#define DHTPIN 32  
#define ALCHO_PIN 34
#define DHTTYPE DHT11   
#define FAN 15

DHT dht(DHTPIN, DHTTYPE);
Adafruit_ST7735 tft = Adafruit_ST7735(13, 12, 14, 27, 0);
Adafruit_AMG88xx amg;

#define AMG88xx_PIXEL_ARRAY_SIZE 8
float pixels[AMG88xx_PIXEL_ARRAY_SIZE * AMG88xx_PIXEL_ARRAY_SIZE];

#define GRID_WIDTH  100
#define GRID_HEIGHT 100
#define GRID_X      0
#define GRID_Y      0

struct ReceivedData {
  int blink_counter;
  bool is_sleeping;
};


// Shared variables
int humidity, prevHumidity;
float temp2=0, prevTemp2=0;
int alcho, prevAlcho;
float prevMax, prevMin, prevAvg;

// Mutex for thread safety
SemaphoreHandle_t xMutex;

// Function to map temperature to color
uint16_t getColor(float val) {
  val = constrain(val, 20, 35);
  uint8_t r, g, b;
  
  if (val < 23) { r = 0; g = map(val, 20, 23, 0, 255); b = 255; } 
  else if (val < 26) { r = 0; g = 255; b = map(val, 23, 26, 255, 0); } 
  else if (val < 29) { r = map(val, 26, 29, 0, 255); g = 255; b = 0; } 
  else { r = 255; g = map(val, 29, 35, 255, 0); b = 0; }

  return tft.color565(r, g, b);
}

// Task 1: Update thermal pixels on TFT
void Task_ThermalGrid(void *pvParameters) {
  while (1) {
    amg.readPixels(pixels);

    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      int pixelSize = GRID_WIDTH / AMG88xx_PIXEL_ARRAY_SIZE;
      
      for (int y = 0; y < AMG88xx_PIXEL_ARRAY_SIZE; y++) {
        for (int x = 0; x < AMG88xx_PIXEL_ARRAY_SIZE; x++) {
          float temp = pixels[y * AMG88xx_PIXEL_ARRAY_SIZE + x];
          uint16_t color = getColor(temp);

          int x0 = GRID_X + x * pixelSize;
          int y0 = GRID_Y + y * pixelSize;

          tft.fillRect(x0, y0, pixelSize, pixelSize, color);
        }
      }
      xSemaphoreGive(xMutex);
    }
    
    vTaskDelay(pdMS_TO_TICKS(100));  // Update every 100ms
  }
}

// Task 2: Display sensor readings without blinking
void Task_DisplayValues(void *pvParameters) {
  while (1) {
    humidity = dht.readHumidity();
    temp2 = dht.readTemperature();
     if (Serial.available() >= sizeof(ReceivedData)) {
      ReceivedData data;
      Serial.readBytes((char*)&data, sizeof(ReceivedData));
      digitalWrite(BUZZER, data.is_sleeping);
     }

    
    alcho = map(analogRead(ALCHO_PIN), 800, 4095, 0, 100);
    float maxTemp = getMaxTemp();
    float minTemp = getMinTemp();
    float avgTemp = getAvgTemp();
    if(avgTemp >= 34) digitalWrite(FAN, HIGH);
    else digitalWrite(FAN, LOW);

    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      tft.setTextSize(1);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Background prevents blinking
      tft.setCursor(GRID_WIDTH + 3, 8);
      tft.print("Thermal:");
      // Only update if value changed
      if (prevMax != maxTemp) {
        tft.setCursor(GRID_WIDTH + 3, 10+10);
        tft.print("Max:"); tft.print(maxTemp, 1);
        //tft.print("Max:"); tft.print(0.00, 1);
        prevMax = maxTemp;
      }

      if (prevMin != minTemp) {
        tft.setCursor(GRID_WIDTH + 3, 20+10);
        tft.print("Min:"); tft.print(minTemp, 1);
        //tft.print("Min:"); tft.print(0.00, 1);

        prevMin = minTemp;
      }

      if (prevAvg != avgTemp) {
        tft.setCursor(GRID_WIDTH + 3, 30+10);
        tft.print("Avg:"); tft.print(avgTemp, 1);
        //tft.print("Avg:"); tft.print(0.00, 1);
        prevAvg = avgTemp;
      }

      if(prevTemp2 != temp2){
        tft.setCursor(GRID_WIDTH + 3, 45+15);
        tft.print("T:"); tft.print(temp2);
        //tft.write(0xB0);
        if(humidity < 10) tft.print(" ");
        prevTemp2 = temp2;
      }

      if (prevHumidity != humidity) {
        tft.setCursor(GRID_WIDTH + 3, 55+15);
        tft.print("Humi:"); tft.print(humidity); tft.print("% ");
        if(humidity < 10) tft.print(" ");
        prevHumidity = humidity;
      }

      if (prevAlcho != alcho) {
        tft.setCursor(GRID_WIDTH + 3, 65+15);
        tft.print("Alcho:"); tft.print(alcho);
        tft.print("%");
        if(alcho < 10) tft.print(" ");
        prevAlcho = alcho;
      }
      

      xSemaphoreGive(xMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));  // Update every 1s
  }
}

// Get max temperature
float getMaxTemp() {
  float maxTemp = pixels[0];
  for (int i = 1; i < AMG88xx_PIXEL_ARRAY_SIZE * AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    if (pixels[i] > maxTemp) {
      maxTemp = pixels[i];
    }
  }
  return maxTemp;
}

// Get min temperature
float getMinTemp() {
  float minTemp = pixels[0];
  for (int i = 1; i < AMG88xx_PIXEL_ARRAY_SIZE * AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    if (pixels[i] < minTemp) {
      minTemp = pixels[i];
    }
  }
  return minTemp;
}

// Get avg temperature
float getAvgTemp() {
  float sum = 0;
  for (int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE * AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    sum += pixels[i];
  }
  return sum / (AMG88xx_PIXEL_ARRAY_SIZE * AMG88xx_PIXEL_ARRAY_SIZE);
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(ALCHO_PIN, INPUT);
  pinMode(FAN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  delay(1000);
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  delay(1000);
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  delay(1000);

  digitalWrite(FAN, LOW);

  

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(1);

  if (!amg.begin()) {
    Serial.println("Could not find AMG88xx sensor!");
    while (1);
  }

  // Create Mutex
  xMutex = xSemaphoreCreateMutex();

  // Create Tasks
  xTaskCreatePinnedToCore(Task_ThermalGrid, "ThermalGrid", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(Task_DisplayValues, "DisplayValues", 4096, NULL, 1, NULL, 1);

  Serial.println("Tasks Started!");
}

void loop() {
  vTaskDelay(portMAX_DELAY);  // Let FreeRTOS handle everything
}
