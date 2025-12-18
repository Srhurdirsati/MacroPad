#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>

#define SW1 26
#define SW2 27
#define SW3 28
#define SW4 29

#define ENC_A 1
#define ENC_B 2
#define ENC_SW 3

#define RGB_PIN 4
#define LED_COUNT 4

#define OLED_SDA 6
#define OLED_SCL 7

Adafruit_NeoPixel leds(LED_COUNT, RGB_PIN, NEO_GRB + NEO_KHZ800);
Encoder encoder(ENC_A, ENC_B);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

long lastPos = 0;
unsigned long lastTapTime = 0;
int tapCount = 0;

bool volumeMode = true;
int rgbMode = 0;
int brightness = 100;

void setRGB() {
  leds.clear();
  if (rgbMode == 0) {
    for (int i = 0; i < LED_COUNT; i++)
      leds.setPixelColor(i, leds.Color(255, 0, 0));
  }
  if (rgbMode == 1) {
    for (int i = 0; i < LED_COUNT; i++)
      leds.setPixelColor(i, leds.Color(0, 255, 0));
  }
  if (rgbMode == 2) {
    for (int i = 0; i < LED_COUNT; i++)
      leds.setPixelColor(i, leds.Color(0, 0, 255));
  }
  leds.setBrightness(brightness);
  leds.show();
}

void updateOLED() {
  display.clearDisplay();
  display.setCursor(0, 0);
  if (volumeMode)
    display.print("Mode: Volume");
  else
    display.print("Mode: Brightness");
  display.setCursor(0, 16);
  display.print("RGB Mode: ");
  display.print(rgbMode + 1);
  display.display();
}

void setup() {
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);

  leds.begin();
  leds.setBrightness(brightness);
  leds.show();

  Wire.setSDA(OLED_SDA);
  Wire.setSCL(OLED_SCL);
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  setRGB();
  updateOLED();

  Keyboard.begin();
}

void loop() {
  if (!digitalRead(SW1)) { Keyboard.write('A'); delay(200); }
  if (!digitalRead(SW2)) { Keyboard.write('B'); delay(200); }
  if (!digitalRead(SW3)) { Keyboard.write('C'); delay(200); }
  if (!digitalRead(SW4)) { Keyboard.write('D'); delay(200); }

  long pos = encoder.read() / 4;
  if (pos != lastPos) {
    if (volumeMode) {
      if (pos > lastPos) Keyboard.write(KEY_MEDIA_VOLUME_UP);
      else Keyboard.write(KEY_MEDIA_VOLUME_DOWN);
    } else {
      if (pos > lastPos && brightness < 255) brightness += 10;
      if (pos < lastPos && brightness > 10) brightness -= 10;
      setRGB();
    }
    lastPos = pos;
  }

  if (!digitalRead(ENC_SW)) {
    if (millis() - lastTapTime < 400) tapCount++;
    else tapCount = 1;

    lastTapTime = millis();
    delay(50);

    while (!digitalRead(ENC_SW));
  }

  if (tapCount > 0 && millis() - lastTapTime > 400) {
    if (tapCount == 1) {
      Keyboard.write(KEY_MEDIA_PLAY_PAUSE);
    }
    if (tapCount == 2) {
      volumeMode = !volumeMode;
      updateOLED();
    }
    if (tapCount == 3) {
      rgbMode++;
      if (rgbMode > 2) rgbMode = 0;
      setRGB();
      updateOLED();
    }
    tapCount = 0;
  }
}
