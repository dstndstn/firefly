#include <CapacitiveSensor.h>

// Capacitive Touch. Digital Pins XX & YY, 1M+ Ohm resistor on XX

#define TOUCH_DELAY 500
//#define TOUCH_THRESH 300
#define TOUCH_THRESH 50


// Touch Sensing
unsigned long mark;
CapacitiveSensor cs1 = CapacitiveSensor(3, 4);

// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

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

bool oldState = HIGH;
int showType = 0;
const int N_SHOWS = 7;

void startShow(int i);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);
void theaterChaseRainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);

void setup();
void loop();

int main() {
    setup();
    for (;;)
        loop();
}

void setup() {

    //Serial.begin(9600);

cs1.set_CS_Timeout_Millis(500);
cs1.reset_CS_AutoCal();
cs1.set_CS_AutocaL_Millis(0xFFFFFFFF);
    
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

/* check capacitive button state */
boolean chkTouch(int touchVal) {
   if (touchVal > TOUCH_THRESH && (millis() - mark) > TOUCH_DELAY) {
       mark = millis();
       return true;
    } 
    else { return false; }
}


void loop() {


    long start = millis();
    
  // if capacitive touch button pressed, advance, set mark
  int touchVal = cs1.capacitiveSensor(30);
  //if (chkTouch(cs1.capacitiveSensor(30))) {
  if (chkTouch(touchVal)) {
      showType++;
      if (showType > 3)
        showType=1;
      startShow(showType);
  }

  //Serial.print(millis() - start);
//Serial.print("\t");                    // tab character for debug window spacing
  //Serial.println(touchVal);
  
  // Get current button state.
  bool newState = digitalRead(BUTTON_PIN);

  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
      showType++;
      if (showType >= N_SHOWS)
        showType=0;
      startShow(showType);
    }
  }

  // Set the last button state to the old state.
  oldState = newState;
}

void startShow(int i) {
  switch(i){
        case 0:
          // scanner, color and delay - RGB
          scanner(strip.Color(255,0,0),50);
          scanner(strip.Color(200,0,100),50);
          scanner(strip.Color(64,0,200),50);
          break;

        case 1:
          // color wipe random RGB
          colorWipe(strip.Color(random(255), random(255), random(255)),50);
        break;

        case 2:
          // color wave - Hue/Sat/Bright
          // hue low (0-359), high (0-359),rate,extra delay
          wavey(200,240,0.06,0);
        break;
          
        case 3:
          // rainbow firefly, 1px at random
          colorFirefly(60);
          counter++;
        break;

        case 4: 
          // rainbow solid
          rainbow(10);
          counter++;
        break;

    case 5: rainbowCycle(20);
            break;
    case 6: theaterChaseRainbow(50);
            break;

            // N_SHOWS

      /*
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(255, 0, 0), 50);  // Red
            break;
    case 2: colorWipe(strip.Color(0, 255, 0), 50);  // Green
            break;
    case 3: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
            break;
    case 4: theaterChase(strip.Color(127, 127, 127), 50); // White
            break;
    case 5: theaterChase(strip.Color(127,   0,   0), 50); // Red
            break;
    case 6: theaterChase(strip.Color(  0,   0, 127), 50); // Blue
            break;
    case 7: rainbow(20);
            break;
    case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;
       */
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
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

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

/*
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
 */

//#define NUM_PATTERNS 5
#define CTR_THRESH 16

// Init Vars
uint8_t j = 0;
uint8_t pattern=1;
uint8_t buttonState=0;
uint8_t lastPix=0; 
uint8_t myPix=0;
uint8_t direction=1;
uint8_t counter=0;
uint8_t colors[3];
uint32_t setColor=0;
unsigned long mark;

/*
loop():
    // if pattern greater than #pattern reset
    if (pattern > NUM_PATTERNS) { pattern = 1; }
    // choose a pattern
    pickPattern(pattern);
    // set direction
    if (direction == 1) { j++;  } else {  j--; }
    if (j > 254) { direction = 0; }
    if (j < 1) { direction = 1; }   
 */

/* pick a pattern 
void pickPattern(uint8_t var) {
      switch (var) {
        case 1:
          // scanner, color and delay - RGB
          scanner(strip.Color(255,0,0),50);
          scanner(strip.Color(200,0,100),50);
          scanner(strip.Color(64,0,200),50);
        break;
        case 2:
          // color wipe random RGB
          colorWipe(strip.Color(random(255), random(255), random(255)),50);
        break;
        case 3:
          // color wave - Hue/Sat/Bright
          // hue low (0-359), high (0-359),rate,extra delay
          wavey(200,240,0.06,0);
        break;
        case 4:
          // rainbow firefly, 1px at random
          colorFirefly(60);
          counter++;
        break;
        case 5:
          // rainbow solid
          rainbow(10);
          counter++;
        break;
      }
}
 */

/* check button state
boolean chkTouch(int touchVal) {
   if (touchVal > TOUCH_THRESH && (millis() - mark) > TOUCH_DELAY) {
       j = 0;
       mark = millis();
       pattern++;
       return true;
    } 
    else { return false; }
}
 */

void colorFirefly(int wait) {
        if(myPix != lastPix) {
          if(counter<CTR_THRESH) {
            float colorV = sin((6.28/30)*(float)(counter)) *255;
            HSVtoRGB((359/CTR_THRESH)*counter, 255, colorV, colors);
            strip.setPixelColor(myPix, colors[0], colors[1], colors[2]);
           strip.show();
           delay(wait);
          } else {
            lastPix=myPix;
            counter=0;
            colorFast(0,0);
          }
        } else {
          myPix=random(0,strip.numPixels());
        }
	
}

// Fill the dots one after the other with a color
// Modified from Neopixel sketch to break on button press
/*
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      if(chkTouch(cs1.capacitiveSensor(30))) { break; }
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
 */

/*
// color wipe from center
void colorWipeCenter(uint32_t c, uint8_t wait) {
  uint8_t mid=strip.numPixels()/2;
  strip.setPixelColor(mid,c);
  for(uint16_t i=0; i<=strip.numPixels()/2; i++) {
      if(chkTouch(cs1.capacitiveSensor(30))) { break; }
      strip.setPixelColor(mid+i, c);
      strip.setPixelColor(mid-i, c);
      strip.show();
      delay(wait);
  }
}
 */

// fast version 
void colorFast(uint32_t c, uint8_t wait) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
    }
    strip.show();
    delay(wait);
}

/*
// Rainbow Cycle, modified from Neopixel sketch to break on button press
void rainbowCycle(uint8_t wait) {
    uint16_t i;

    //  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
    // }
}
 */

/*
void rainbow(uint8_t wait) {
    uint16_t i;

    //for(j=0; j<256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
    // }
}
 */
// scanner

void scanner(uint32_t c,uint8_t wait) {
        for(int i=0; i< strip.numPixels(); i++) {
            //  if(chkTouch(cs1.capacitiveSensor(30))) { break; }

            colorFast(0,0);
            strip.setPixelColor(i,c);
            strip.show();
            delay(wait);
        }
        for(int i=strip.numPixels(); i>0; i--) {
            // if(chkTouch(cs1.capacitiveSensor(30))) { break; }
            colorFast(0,0);
            strip.setPixelColor(i,c);
            strip.show();
            delay(wait);
        }    
}

// scanner to midpoint
void bounceInOut(uint8_t num, uint8_t start,uint8_t wait) {
  colorFast(0,0);
  uint8_t color=200;
  for(int q=0; q < num; q++) {
    if(strip.numPixels()-start >= 0 && start < NUM_LEDS) {
          strip.setPixelColor(start+q, strip.Color(0,color,0));
          strip.setPixelColor(strip.numPixels()-start-q, strip.Color(0,color,0));
      }   
    color=round(color/2.0);
    strip.show();
    delay(wait);
  }
  if(counter > strip.numPixels()) { counter=0; }
}

void fadeEveOdd(int c1,byte rem,uint8_t wait) {
              for(int j=0; j < CTR_THRESH; j++) {
                      for(int i=0; i< strip.numPixels(); i++) {
                        if(i % 2== rem) {
                           HSVtoRGB(c1,255,(255/CTR_THRESH)*j,colors);
                           strip.setPixelColor(i,colors[0],colors[1],colors[2]);
                         }
                      }           
                      //                      if(chkTouch(cs1.capacitiveSensor(30))) { break; }
                      strip.show();
                      delay(wait);
                }
                for(int j=CTR_THRESH; j >= 0; j--) {
                      for(int i=0; i< strip.numPixels(); i++) {
                        if(i % 2== rem) {
                           HSVtoRGB(c1,255,(255/CTR_THRESH)*j,colors);
                           strip.setPixelColor(i,colors[0],colors[1],colors[2]);
                         }
                      }             
                      //if(chkTouch(cs1.capacitiveSensor(30))) { break; }
                      strip.show();
                      delay(wait);
                } 
}

// twinkle random number of pixels
void twinkleRand(int num,uint32_t c,uint32_t bg,int wait) {
	// set background
	 colorFast(bg,0);
	 // for each num
	 for (int i=0; i<num; i++) {
	   strip.setPixelColor(random(strip.numPixels()),c);
	 }
	strip.show();
	delay(wait);
}

// sine wave, low (0-359),high (0-359), rate of change, wait
void wavey(int low,int high,float rt,uint8_t wait) {
  float in,out;
  int diff=high-low;
  int step = diff/strip.numPixels();
  for (in = 0; in < 6.283; in = in + rt) {
       for(int i=0; i< strip.numPixels(); i++) {
           out=sin(in+i*(6.283/strip.numPixels())) * diff + low;
           HSVtoRGB(out,255,255,colors);
           strip.setPixelColor(i,colors[0],colors[1],colors[2]);
       }
           strip.show();
           delay(wait);
           //           if(chkTouch(cs1.capacitiveSensor(30))) { break; }
  }
}

// sine wave, intensity
void waveIntensity(float rt,uint8_t wait) {
  float in,level;
  for (in = 0; in < 6.283; in = in + rt) {
       for(int i=0; i< strip.numPixels(); i++) {
         // sine wave, 3 offset waves make a rainbow!
        level = sin(i+in) * 127 + 128;
        // set color level 
        strip.setPixelColor(i,(int)level,0,0);
       }
           strip.show();
           delay(wait);
           //           if(chkTouch(cs1.capacitiveSensor(30))) { break; }
  }
}

// helpers 

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



