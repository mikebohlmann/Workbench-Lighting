#include <Encoder.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 6
#define PIN_ENCODER_SWITCH 4

Encoder myEnc(2, 3);
int stripStatus = 0;
long oldPosition  = -999;
int pos = 30;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIXEL_PIN, NEO_WGRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  strip.begin();
  allOff(); // Initialize all pixels to 'off'

  pinMode(PIN_ENCODER_SWITCH, INPUT_PULLUP);
}

void loop() {

  long newPosition = myEnc.read();

  if (stripStatus == 1) {
    if (newPosition != oldPosition) {
      int diff = newPosition - oldPosition;
      if (diff > 0) {
        brightAll(diff);
      } 
      if (diff < 0) {
        dimAll(diff);
      }
      oldPosition = newPosition;
      Serial.println(newPosition);
    }
  }

  if (stripStatus == 2) {
    if (newPosition != oldPosition) {
      int diff = newPosition - oldPosition;
      pos += diff;
      if (pos > 60) {
        pos = 60;
      }
      if (pos < 0) {
        pos = 0;
      }
      spotLightShift(pos);
      oldPosition = newPosition;
      Serial.println(newPosition);      
    }
  }

  int val = digitalRead(PIN_ENCODER_SWITCH);
  if (val == 0) {
    if (stripStatus < 3) {
      stripStatus++;
    } else {
      stripStatus = 0;
    }

    switch (stripStatus) {
    case 0:
      allOff();
      break;
    case 1:
      allOn();
      break;
    case 2:
      spotLightInit();
      break; 
    case 3:
      break;
    default:
      break;
    }

    delay(500);
  }

  if (stripStatus == 3) {
    danceParty();
  }
  
}

void spotLightShift(int pos) {
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

void spotLightInit() {
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

void danceParty() {
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 127, 0), 50); // Green
  theaterChase(strip.Color(0, 0, 127), 50); // Blue
}

void dimAll(int diff) {
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

void brightAll(int diff) {
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

void allOff() {
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);        //turn every pixel off
  }

  strip.show();
}

void allOn() {
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 255);        //turn every pixel on
  }

  strip.show();
}

void showoff() {
  colorWipe(strip.Color(255, 0, 0), 25);
  colorWipe(strip.Color(0, 255, 0), 25);
  colorWipe(strip.Color(0, 0, 255), 25);
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

