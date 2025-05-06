/*************
Rajib Hasan

*************/

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define ENABLE_MAX30100 1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 //64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET    4 // 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(128, 32, &Wire, -1);

//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#if ENABLE_MAX30100
#define REPORTING_PERIOD_MS     5000
// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;
#endif
uint32_t tsLastReport = 0;
int xPos = 0;
// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  Serial.println("Beat!");
  heart_beat(&xPos);
}
void setup()
{
  Serial.begin(115200);
  Serial.println("SSD1306 128x64 OLED TEST");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  //display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 18);
  // Display static text
  display.print("Pulse OxiMeter");
  int temp1 = 0;
  int temp2 = 40;
  int temp3 = 80;
  heart_beat(&temp1);
  heart_beat(&temp2);
  heart_beat(&temp3);
  xPos = 0;
  display.display();
  delay(2000); // Pause for 2 seconds
  display.cp437(true);
  display.clearDisplay();
  Serial.print("Initializing pulse oximeter..");
#if ENABLE_MAX30100
  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
  display_data(0, 0);
#endif
}
void loop()
{
#if ENABLE_MAX30100
  // Make sure to call update as fast as possible
  pox.update();
  int bpm = 0;
  int spo2 = 0;
  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    //Serial.print("Heart rate:");
    bpm = pox.getHeartRate();
    spo2 = pox.getSpO2();
    Serial.println(bpm);
    //Serial.print("bpm / SpO2:");
    Serial.println(spo2);
    //Serial.println("%");
    tsLastReport = millis();
    display_data(bpm, spo2);
  }
#endif
  drawLine(&xPos);
}
void display_data(int bpm, int spo2) {

display.fillRect(0, 18, 127, 15, BLACK);
  //if(bpm ==0 && spo2==0){
 //    display.setTextSize(1);
 // display.setTextColor(WHITE);
  //display.setCursor(0, 18);
  // Display static text
 // display.print("Fingure Out");
  //}
  display.fillRect(0, 18, 127, 15, BLACK);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 18);
  // Display static text
  display.print("BPM ");
  display.setTextSize(2);
  display.print(bpm);
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(64, 18);
  // Display static text
  display.print("Spo2% ");
  display.setTextSize(2);
  display.println(spo2);
  display.display();
}
void drawLine(int *x_pos) {
  // Draw a single pixel in white
  display.drawPixel(*x_pos, 8, WHITE);
  display.drawPixel((*x_pos)++, 8, WHITE);
  display.drawPixel((*x_pos)++, 8, WHITE);
  display.drawPixel((*x_pos)++, 8, WHITE);
  display.drawPixel((*x_pos), 8, BLACK);  // -----
  //Serial.println(*x_pos);
  display.fillRect(*x_pos, 0, 31, 16, BLACK);
  display.display();
  //delay(1);
  if (*x_pos >= SCREEN_WIDTH) {
    *x_pos = 0;
  }
}
void heart_beat(int *x_pos) {
  /************************************************/
  //display.clearDisplay();
  display.fillRect(*x_pos, 0, 30, 15, BLACK);
  // Draw a single pixel in white
  display.drawPixel(*x_pos + 0, 8, WHITE);
  display.drawPixel(*x_pos + 1, 8, WHITE);
  display.drawPixel(*x_pos + 2, 8, WHITE);
  display.drawPixel(*x_pos + 3, 8, WHITE);
  display.drawPixel(*x_pos + 4, 8, BLACK); // -----
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 5, 7, WHITE);
  display.drawPixel(*x_pos + 6, 6, WHITE);
  display.drawPixel(*x_pos + 7, 7, WHITE); // .~.
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 8, 8, WHITE);
  display.drawPixel(*x_pos + 9, 8, WHITE); // --
  //display.display();
  //delay(1);
  /******************************************/
  display.drawPixel(*x_pos + 10, 8, WHITE);
  display.drawPixel(*x_pos + 10, 9, WHITE);
  display.drawPixel(*x_pos + 11, 10, WHITE);
  display.drawPixel(*x_pos + 11, 11, WHITE);
  //display.display();
  //delay(1);
  /******************************************/
  display.drawPixel(*x_pos + 12, 10, WHITE);
  display.drawPixel(*x_pos + 12, 9, WHITE);
  display.drawPixel(*x_pos + 12, 8, WHITE);
  display.drawPixel(*x_pos + 12, 7, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 13, 6, WHITE);
  display.drawPixel(*x_pos + 13, 5, WHITE);
  display.drawPixel(*x_pos + 13, 4, WHITE);
  display.drawPixel(*x_pos + 13, 3, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 14, 2, WHITE);
  display.drawPixel(*x_pos + 14, 1, WHITE);
  display.drawPixel(*x_pos + 14, 0, WHITE);
  display.drawPixel(*x_pos + 14, 0, WHITE);
  //display.display();
  //delay(1);
  /******************************************/
  display.drawPixel(*x_pos + 15, 0, WHITE);
  display.drawPixel(*x_pos + 15, 1, WHITE);
  display.drawPixel(*x_pos + 15, 2, WHITE);
  display.drawPixel(*x_pos + 15, 3, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 15, 4, WHITE);
  display.drawPixel(*x_pos + 15, 5, WHITE);
  display.drawPixel(*x_pos + 16, 6, WHITE);
  display.drawPixel(*x_pos + 16, 7, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 16, 8, WHITE);
  display.drawPixel(*x_pos + 16, 9, WHITE);
  display.drawPixel(*x_pos + 16, 10, WHITE);
  display.drawPixel(*x_pos + 16, 11, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 17, 12, WHITE);
  display.drawPixel(*x_pos + 17, 13, WHITE);
  display.drawPixel(*x_pos + 17, 14, WHITE);
  display.drawPixel(*x_pos + 17, 15, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 18, 15, WHITE);
  display.drawPixel(*x_pos + 18, 14, WHITE);
  display.drawPixel(*x_pos + 18, 13, WHITE);
  display.drawPixel(*x_pos + 18, 12, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 19, 11, WHITE);
  display.drawPixel(*x_pos + 19, 10, WHITE);
  display.drawPixel(*x_pos + 19, 9, WHITE);
  display.drawPixel(*x_pos + 19, 8, WHITE);
  //display.display();
  //delay(1);
  /****************************************************/
  display.drawPixel(*x_pos + 20, 8, WHITE);
  display.drawPixel(*x_pos + 21, 8, WHITE);
  //display.display();
  //delay(1);
  /****************************************************/
  display.drawPixel(*x_pos + 22, 7, WHITE);
  display.drawPixel(*x_pos + 23, 6, WHITE);
  display.drawPixel(*x_pos + 24, 6, WHITE);
  display.drawPixel(*x_pos + 25, 7, WHITE);
  //display.display();
  //delay(1);
  /************************************************/
  display.drawPixel(*x_pos + 26, 8, WHITE);
  display.drawPixel(*x_pos + 27, 8, WHITE);
  display.drawPixel(*x_pos + 28, 8, WHITE);
  display.drawPixel(*x_pos + 29, 8, WHITE);
  display.drawPixel(*x_pos + 30, 8, WHITE); // -----
  *x_pos = *x_pos + 30;
  display.display();
  delay(1);
}
