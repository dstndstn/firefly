#include <CapacitiveSensor.h>

// Capacitive Touch. Digital Pins XX & YY, 1M+ Ohm resistor on XX

#define TOUCH_DELAY 500
//#define TOUCH_THRESH 300
#define TOUCH_THRESH 50

// Touch Sensing
unsigned long mark;
CapacitiveSensor cs1 = CapacitiveSensor(3, 4);

#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN   2    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.

#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 3

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_RGB + NEO_KHZ800);


uint32_t Wheel(byte WheelPos);
void HSVtoRGB(int hue, int sat, int val, uint8_t * colors);

void setup();
void loop();

#if STANDALONE
// From https://github.com/arduino/Arduino/blob/2bfe164b9a5835e8cb6e194b928538a9093be333/hardware/arduino/avr/cores/arduino/main.cpp
// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

int main() {
  init();
  initVariant();
  setup();
    for (;;)
        loop();
}
#endif


void setup() {

#if SERIAL
  Serial.begin(9600);
#endif

  cs1.set_CS_Timeout_Millis(500);
  cs1.reset_CS_AutoCal();
  cs1.set_CS_AutocaL_Millis(0xFFFFFFFF);
    
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

/* debounce capacitive button.  uses global 'mark' */
boolean chkTouch(int touchVal) {
   if (touchVal > TOUCH_THRESH && (millis() - mark) > TOUCH_DELAY) {
       mark = millis();
       return true;
    } 
    return false;
}

class LedShow {
 public:
    LedShow(uint16_t n_leds) : n(n_leds) {}
    ~LedShow() {}
    virtual void step() {}
 protected:
    uint16_t n;
};

class RainbowCycle : public LedShow {
 public:
    RainbowCycle(uint16_t n_leds) :
        LedShow(n_leds),
        stepnum(0) {
    }

    virtual void step() {
        uint16_t i;
        this->stepnum++;
        for(i=0; i<this->n; i++) {
            strip.setPixelColor(i, Wheel(((i * 256 / this->n) + this->stepnum) % 255));
        }
        strip.show();
    }

 protected:
    uint16_t stepnum;
};

class Blinker : public LedShow {
 public:
    Blinker(uint16_t n_leds) :
        LedShow(n_leds),
        stepnum(0) {
        hues = (uint16_t*)malloc(n_leds * sizeof(uint16_t));
        sats = (uint16_t*)malloc(n_leds * sizeof(uint16_t));
    }

    ~Blinker() {
        free(hues);
        free(sats);
    }

    virtual void step() {
        uint16_t i;
        for(i=0; i<this->n; i++) {
            int phase = (i * 256 / this->n) + this->stepnum;
            if (phase % 256 == 0) {
                // select new random Hue & Sat for this LED
                this->hues[i] = random(360);
                this->sats[i] = random(128) + 128;
            }
            // Lightness varies as a (co)sine wave with period 256.
            uint8_t val = 128 - 127 * cos(2.0 * PI * phase / 256.0);
            uint8_t rgb[3];
            HSVtoRGB(this->hues[i], this->sats[i], val, rgb);
            strip.setPixelColor(i, rgb[0], rgb[1], rgb[2]);
        }
        strip.show();
        this->stepnum++;
    }

 protected:
    uint16_t stepnum;
    uint16_t* hues;
    uint16_t* sats;
};



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r. (???)
uint32_t Wheel(byte WheelPos) {
    if (WheelPos < 85) {
        return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}


// HSV to RGB colors
// hue: 0-359, sat: 0-255, val (lightness): 0-255
// adapted from http://funkboxing.com/wordpress/?p=1366
void HSVtoRGB(int hue, int sat, int val, uint8_t * colors) {
    int r, g, b, base;
    if (sat == 0) { // Achromatic color (gray).
        colors[0] = val;
        colors[1] = val;
        colors[2] = val;
    } else {
        base = ((255 - sat) * val) >> 8;
        switch (hue / 60) {
        case 0:
            colors[0] = val;
            colors[1] = (((val - base) * hue) / 60) + base;
            colors[2] = base;
            break;
        case 1:
            colors[0] = (((val - base) * (60 - (hue % 60))) / 60) + base;
            colors[1] = val;
            colors[2] = base;
            break;
        case 2:
            colors[0] = base;
            colors[1] = val;
            colors[2] = (((val - base) * (hue % 60)) / 60) + base;
            break;
        case 3:
            colors[0] = base;
            colors[1] = (((val - base) * (60 - (hue % 60))) / 60) + base;
            colors[2] = val;
            break;
        case 4:
            colors[0] = (((val - base) * (hue % 60)) / 60) + base;
            colors[1] = base;
            colors[2] = val;
            break;
        case 5:
            colors[0] = val;
            colors[1] = base;
            colors[2] = (((val - base) * (60 - (hue % 60))) / 60) + base;
            break;
        }

    }
}



bool oldState = HIGH;

RainbowCycle rainbow(PIXEL_COUNT);
Blinker blinker(PIXEL_COUNT);
LedShow* ledshow = &blinker;

void loop() {

    ledshow->step();
    delay(20);

  return;

#if SERIAL
  long start = millis();
#endif

  // if capacitive touch button pressed, advance, set mark
  int touchVal = cs1.capacitiveSensor(30);
  if (chkTouch(touchVal)) {
    // PRESSED
  }  

#if SERIAL
  Serial.print(millis() - start);
  Serial.print("\t");
  Serial.println(touchVal);
#endif  

  // Read & debounce the normal button
  bool newState = digitalRead(BUTTON_PIN);
  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
        // PRESSED
    }
  }
  // Set the last button state to the old state.
  oldState = newState;
}



