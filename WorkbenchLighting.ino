#include <Encoder.h>
#include <Adafruit_NeoPixel.h>

// Pin for the Neopixel strip connction
#define NEOPIXEL_PIN 6
// Pin for the encoder's push button switch
#define PIN_ENCODER_SWITCH 2

int switchInterrupt = 0;

// Initialize the encoder connected to these two pins
Encoder myEnc(3, 9);
volatile int stripStatus = 0;
volatile int debounce = 0;
volatile long oldPosition  = -999;
volatile long newPosition = 999;
int pos = 30;
int danceBeats = 50;

// Initialize the Neopixels
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIXEL_PIN, NEO_WGRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  // Seed random number generator
  randomSeed(analogRead(0));

  // Initialize all pixels to 'off'
  strip.begin();
  allOff(); 

  pinMode(PIN_ENCODER_SWITCH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_SWITCH), modechange, RISING);
}

void loop() {

  // checks for a new position of the rotary encoder
  newPosition = myEnc.read();

  // do an action based on the current system status
  // 0 = off
  // 1 = on
  // 2 = spotlight
  // 3 = dance party
  
  if (stripStatus == 1) { // on adjusts brightness
    if (newPosition != oldPosition) {
      int diff = newPosition - oldPosition;
      if (diff > 0) {
        brightAll(diff); // brighten all the lights
      } 
      if (diff < 0) {
        dimAll(diff); // dim all the lights
      }
      oldPosition = newPosition;
    }
  }

  if (stripStatus == 2) { // spotlight adjusts position
    if (newPosition != oldPosition) {
      int diff = newPosition - oldPosition;
      pos += diff;
      if (pos > 60) {
        pos = 60;
      }
      if (pos < 0) {
        pos = 0;
      }
      spotLightShift(pos); // change the position of the spotlight
      oldPosition = newPosition;     
    }
  }

  if (stripStatus == 3) { // change the beats per minute in dance party 
    if (newPosition != oldPosition) {
      int diff = newPosition - oldPosition;
      danceBeats += diff * 5;
      if (danceBeats > 500) {
        danceBeats = 500;
      }
      if (danceBeats < 30) {
        danceBeats = 30;
      }
      
      oldPosition = newPosition;     
    }

    Serial.println("Beats: " + String(danceBeats));
  }

  // repeats this as long as it's in mode 3 dance party
  if (stripStatus == 3) {
    danceParty(danceBeats);
  }

  
}

void modechange() {

  int rightNow = millis();
  if (rightNow - debounce > 300) {

    debounce = rightNow;
    if (stripStatus < 3) {
      stripStatus++;
    } else {
      stripStatus = 0;
    }


    switch (stripStatus) {
    case 0:
      allOff();
      Serial.println("Turn all off");
      break;
    case 1:
      allOn();
      Serial.println("Turn all on");
      break;
    case 2:
      spotLightInit();
      Serial.println("Turn on spotlight");
      break; 
    case 3:
      Serial.println("Setup dance party mode");
      break;
    default:
      break;
    }

    Serial.println("Status: " + String(stripStatus));
    
  }

}

void spotLightShift(int pos) { // shifts the position of the spotlight
  int pixels = strip.numPixels();
  for (int i = 0; i < pixels; i++) {
    if (i < pos - 8) {
      strip.setPixelColor(i, 0);
    }
    if (i >= pos - 8 && i <= pos + 8) {
      strip.setPixelColor(i, 255);
    }
    if (i > pos + 8) {
      strip.setPixelColor(i, 0);
    }
  }
  strip.show();
}

void spotLightInit() { // initializes the spotlight
  int pixels = strip.numPixels();
  for (int i = 0; i < pixels; i++) {
    if (i < (pixels / 2) - 8) {
      strip.setPixelColor(i, 0);
    }
    if (i >= (pixels / 2) - 8 && i <= (pixels / 2) + 8) {
      strip.setPixelColor(i, 255);
    }
    if (i > (pixels / 2) + 8) {
      strip.setPixelColor(i, 0);
    }
  }
  strip.setBrightness(255);
  strip.show();
}

void danceParty(int beats) { // whomp whomp whomp
  // Send a theater pixel chase in...
  long randNumber;

  randNumber = random(1, 4);
  switch (randNumber) {
    case 1:
      theaterChase(strip.Color(127, 127, 127), beats); // White
      break;
    case 2:
      theaterChase(strip.Color(127, 0, 0), beats); // Red
      break;
    case 3:
      theaterChase(strip.Color(0, 127, 0), beats); // Green
    case 4:
      theaterChase(strip.Color(0, 0, 127), beats); // Blue
      break;
    default:
      break;
  }
  
}

void dimAll(int diff) { // dims all the lights
  int brightness = strip.getBrightness();
  if (brightness > 1) {
    brightness += diff;
    if (brightness < 1) {
      brightness = 1;
    }
    strip.setBrightness(brightness);
    strip.show();
  }
}

void brightAll(int diff) { // brightens all the lights
  int brightness = strip.getBrightness();
  if (brightness < 256) {
    brightness += diff;
    if (brightness > 255) {
      brightness = 255;
    }
    strip.setBrightness(brightness);
    strip.show();
  }

}

void allOff() { // turns off all the lights
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);        //turn every pixel off
  }

  strip.show();
}

void allOn() { // turns on all the lights
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 255);        //turn every pixel on
  }

  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<1; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

