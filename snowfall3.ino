#include <WS2811Driver.h>

#define NUM_LEDS 60
#define NUM_CHANNELS 3 * NUM_LEDS

byte leds[NUM_CHANNELS];
WS2811Driver ledStrip; // uses P1_7 as datapin connect to DIN on strip

int fills[NUM_LEDS];

int depth = NUM_LEDS;
int max_weight = 8;
int baseline_intensity = 5;

const int buttonPin = PUSH2;     // the number of the pushbutton pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
byte clickCounter = 0;

boolean running = true;

void setup(void)
{
  pinMode(buttonPin, INPUT);

  ledStrip.setLEDCount(NUM_LEDS); // setup for 60 leds on a strip
  ledStrip.begin();   // configure P1.7 for output

  // clear out any lights
  clear();
}

void clear() {
  // clear out any lights
  for (byte x = 0 ; x < NUM_CHANNELS ; x++){
    leds[x] = false;
  }
  ledStrip.write(leds);	// write to the LEDs

  // initialize the list of filled lights
  for (int x = 0 ; x < NUM_LEDS ; x++){
    fills[x] = 0;
  }
  depth = NUM_LEDS;
}

void loop() {

  // check the button to see if it has been pressed
  buttonCheck();
  
  if ( running ) {
    snow_fall(80);
    // if we fill up, clear it all out
    if (depth == 1) {
      clear();
    }
  }
}

void buttonCheck() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState = reading;
    
    clickCounter += 1;
    if (clickCounter > 5) {
      clickCounter = 0;
    }

    if (clickCounter % 2 == 0) {
      running = !running;
    }
    if (!running) {
      // clear();
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}

void snow_fall(int idelay) {

  int top_of_pile = depth - 1;
  
  // figure out what should be filled
  for (int i = top_of_pile; i > 0; i--) {

    int current = fills[i];
    int prior = fills[i - 1];

    if (i == top_of_pile) {
      fills[i] += prior;
      if (fills[i] == max_weight) {
        // then we need to alter the depth
        depth -= 1; 
      }
    } else {
      fills[i] = prior;
    }
  }
  

  // decide if a snowflake will fall
  snow_generator();

  // now, given the fills, light up the lights
  for (byte x = 1 ; x < depth ; x++) {
    // for each one, check the weight
    int f = fills[x];
    int rgb = baseline_intensity * f;
    byte basePos = (3 * x);
    leds[basePos]       = rgb;
    leds[basePos + 1]   = rgb;
    leds[basePos + 2]   = rgb;
  }

  ledStrip.write(leds);   // write to the LEDs
  delay(idelay);
}

void snow_generator() {
  // randomly (and seldomly) start a new snowflake 
  byte r = random(0, 110);
  byte r2 = random(0, 37);
  if (r % r2 == 0) {
    leds[0] = baseline_intensity;
    leds[1] = baseline_intensity;
    leds[2] = baseline_intensity;
    fills[0] = 1;
  } 
  else {
    leds[0] = 0; 
    leds[1] = 0; 
    leds[2] = 0; 
    fills[0] = 0;
  }
}

//-CONVERT HSV VALUE TO RGB
void HSVtoRGB(int hue, int sat, int val, int colors[3]) {
  // hue: 0-359, sat: 0-255, val (lightness): 0-255
  int r, g, b, base;

  if (sat == 0) { // Achromatic color (gray).
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;
  } 
  else  {
    base = ((255 - sat) * val)>>8;
    switch(hue/60) {
    case 0:
      r = val;
      g = (((val-base)*hue)/60)+base;
      b = base;
      break;
    case 1:
      r = (((val-base)*(60-(hue%60)))/60)+base;
      g = val;
      b = base;
      break;
    case 2:
      r = base;
      g = val;
      b = (((val-base)*(hue%60))/60)+base;
      break;
    case 3:
      r = base;
      g = (((val-base)*(60-(hue%60)))/60)+base;
      b = val;
      break;
    case 4:
      r = (((val-base)*(hue%60))/60)+base;
      g = base;
      b = val;
      break;
    case 5:
      r = val;
      g = base;
      b = (((val-base)*(60-(hue%60)))/60)+base;
      break;
    }
    colors[0]=r;
    colors[1]=g;
    colors[2]=b;
  }
}

