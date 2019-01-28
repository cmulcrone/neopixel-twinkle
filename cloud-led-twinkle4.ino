#include <Adafruit_NeoPixel.h>

#define PIN 1
#define BUTTON_PIN 2
#define NUM_LEDS 12
#define COLOR_PRESET 0xFFFFFFFF    // whiteish
//#define COLOR_PRESET 0xFF0000FF
//                     N/A | R | G | B
//                      If the starting values are uneven, that's why you'll see the color fade to a primary
//#define COLOR_PRESET 0x00F02000    // orange
//TODO - set COLOR_PRESET to an array and change the reset section in the igniter to select randomly from the array size

// Parameter 1 = number of Pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA Pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA Pixels, not v2)
//   NEO_GRBW    Pixels are wired for GRBW bitstream (SK6812 RGBW LEDs)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

//State array per color for each LED
uint8_t rStates[NUM_LEDS];
uint8_t bStates[NUM_LEDS];
uint8_t gStates[NUM_LEDS];
float fadeRate = 0.05;
uint8_t igniteRate = 30;
uint8_t  rPreset = ((COLOR_PRESET >> 16) & 0xff);
uint8_t  gPreset = ((COLOR_PRESET >>  8) & 0xff);
uint8_t  bPreset = ((COLOR_PRESET      ) & 0xff);

int ledMode = 0;
unsigned long keyPrevMillis = 0;
const unsigned long keySampleIntervalMs = 25;
byte longKeyPressCountMax = 80;    // 80 * 25 = 2000 ms
byte longKeyPressCount = 0;

byte prevKeyState = HIGH;         // button is active low

void setup() {
  strip.begin();
  strip.show();
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    rStates[i] = 0;
    gStates[i] = 0;
    bStates[i] = 0;
  }
  strip.setBrightness(128);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop () {
  //Igniter
  //Randomly choose to check if a given LED's state is off across all channels
  if (random(igniteRate) == 1) {
    //Select random LED
    uint16_t i = random(NUM_LEDS);
    //If selected LED number's color states are off across all channels, set the RGB channels to the preset color
    if (rStates[i] < 1 && gStates[i] < 1 && bStates[i] < 1) {
      //checkPalleteSwitch();
      rStates[i] = rPreset;
      gStates[i] = gPreset;
      bStates[i] = bPreset;
    }
  }

  //Fader
  //Loop through each LED
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    checkPalleteSwitch();
    //Set the selected LED's (i) RGB channel values based on the previous iteration
    strip.setPixelColor(i, rStates[i], gStates[i], bStates[i]);
    //If any of the RGB channels are on, then begin fading routine
    if ( rStates[i] >= 1 || gStates[i] >= 1 || bStates[i] >= 1 ) {
      if ( rStates[i] >= 1 ) {
        rStates[i] -= rStates[i] * fadeRate;
      } else {
        rStates[i] = 0;
      }
      if ( gStates[i] >= 1 ) {
        gStates[i] -= gStates[i] * fadeRate;
      } else {
        gStates[i] = 0;
      }
      if ( bStates[i] >= 1 ) {
        bStates[i] -= bStates[i] * fadeRate;
      } else {
        bStates[i] = 0;
      }
    }
  }
  //Show current strip state
  strip.show();
  delay(50);
}

void shortKeyPress() {
  ledMode++;
  if (ledMode > 7) {
    ledMode = 0;
  }
}

void readButtonState() {
  byte currKeyState = digitalRead(BUTTON_PIN);

  if ((prevKeyState == LOW) && (currKeyState == HIGH)) {
    shortKeyPress();
  }
  prevKeyState = currKeyState;
}

void checkPalleteSwitch() {
 readButtonState();

  switch (ledMode) {
    case 0: //White
      //currentPalette = HeatColors_p;    //Red & Yellow, Fire Colors
      rPreset = ((0xFFFFFFFF >> 16) & 0xff);
      gPreset = ((0xFFFFFFFF >>  8) & 0xff);
      bPreset = ((0xFFFFFFFF      ) & 0xff);
      break;
    case 1: //Red
      rPreset = ((0xFFFF0000 >> 16) & 0xff);
      gPreset = ((0xFFFF0000 >>  8) & 0xff);
      bPreset = ((0xFFFF0000      ) & 0xff);
      break;
    case 2: //Orange
      rPreset = ((0xF0F02000 >> 16) & 0xff);
      gPreset = ((0xF0F02000 >>  8) & 0xff);
      bPreset = ((0xF0F02000      ) & 0xff);
      break;
    case 3: //Green
      rPreset = ((0x0000FF00 >> 16) & 0xff);
      gPreset = ((0x0000FF00 >>  8) & 0xff);
      bPreset = ((0x0000FF00      ) & 0xff);
      break;
    case 4:  //Blue
      rPreset = ((0x000000FF >> 16) & 0xff);
      gPreset = ((0x000000FF >>  8) & 0xff);
      bPreset = ((0x000000FF      ) & 0xff);
      break;
    case 5: //Indigo
      rPreset = ((0xF0F000FF >> 16) & 0xff);
      gPreset = ((0xF0F000FF >>  8) & 0xff);
      bPreset = ((0xF0F000FF      ) & 0xff);
      break;
    case 6: //Sparkle
      Sparkle(0xff, 0xff, 0xff, 2);
      break;
    case 7: //Rainbow
      //      strip.setBrightness(120);
      rainbow(30);
      delay(10);
      break;

  }
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  strip.setPixelColor(Pixel, red, green, blue);
  strip.show();
  delay(SpeedDelay);
  strip.setPixelColor(Pixel, 0, 0, 0);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i * 1 + j) & 255));
    }
    strip.show();
    delay(wait);
    readButtonState();
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
