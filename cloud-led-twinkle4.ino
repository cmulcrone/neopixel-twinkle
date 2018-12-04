#include <Adafruit_NeoPixel.h>

#define PIN 1
#define BUTTON_PIN 2
#define NUM_LEDS 16
#define COLOR_PRESET 0x11FF55AA    // whiteish
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
float fadeRate = 0.02;
uint8_t igniteRate = 30;
uint8_t  rPreset = ((COLOR_PRESET >> 16)&0xff);
uint8_t  gPreset = ((COLOR_PRESET >>  8)&0xff);
uint8_t  bPreset = ((COLOR_PRESET      )&0xff);

int ledMode = 0;
unsigned long keyPrevMillis = 0;
const unsigned long keySampleIntervalMs = 25;
byte longKeyPressCountMax = 80;    // 80 * 25 = 2000 ms
byte longKeyPressCount = 0;

byte prevKeyState = HIGH;         // button is active low

void setup() {
  strip.begin();
  strip.show();
  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    rStates[i] = 0;
    gStates[i] = 0;
    bStates[i] = 0;
  }
}

void loop () {
    byte currKeyState = digitalRead(BUTTON_PIN);
 
  if ((prevKeyState == LOW) && (currKeyState == HIGH)) {
    shortKeyPress();
  }
  prevKeyState = currKeyState;
 
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
 
  switch (ledMode) {
 
  case 0:
    //currentPalette = HeatColors_p;    //Red & Yellow, Fire Colors
    break;
  case 1:
    //currentPalette = ForestColors_p;    //Foresty greens and yellows
    break;
  case 2: 
    //currentPalette = OceanColors_p;  //Oceans are pretty and filled with mermaids
    break;
  case 3: 
    //currentPalette = PurpleColors_p;  //My custom palette from above
    break;
  case 4:
    //currentPalette = RainbowColors_p;  //All the colors!
    break;
  case 5:
    //currentPalette = RainbowStripeColors_p;   //Rainbow stripes
    break;      
  case 6:
    //currentPalette = PartyColors_p; //All the colors except the greens, which make people look a bit washed out
    break;
  } 
    
    //Igniter
    //Randomly choose to check if a given LED's state is off across all channels
    if (random(igniteRate) == 1) {
      //Select random LED
      uint16_t i = random(NUM_LEDS);
      //If selected LED number's color states are off across all channels, set the RGB channels to the preset color
      if (rStates[i] < 1 && gStates[i] < 1 && bStates[i] < 1) {
        rStates[i] = rPreset;
        gStates[i] = gPreset;
        bStates[i] = bPreset;
      }
    }

    //Fader
    //Loop through each LED
    for(uint16_t i = 0; i < NUM_LEDS; i++) {
      //Set the selected LED's (i) RGB channel values based on the previous iteration
      strip.setPixelColor(i, rStates[i], gStates[i], bStates[i]);
      //If any of the RGB channels are on, then begin fading routine
      if ( rStates[i] >= 1 || gStates[i] >= 1 || bStates[i] >= 1 ){
        if ( rStates[i] >= 1 ){
          rStates[i] -= rStates[i]*fadeRate;
        } else {
          rStates[i] = 0;
        }
        if ( gStates[i] >= 1 ){
          gStates[i] -= gStates[i]*fadeRate;
        } else {
          gStates[i] = 0;
        }
        if ( bStates[i] >= 1 ){
          bStates[i] -= bStates[i]*fadeRate;
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
  if (ledMode > 6) {
    ledMode=0; 
  }  
}
