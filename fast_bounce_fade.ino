#include <WS2811Driver.h>

#define NUM_LEDS 60
#define NUM_CHANNELS 3 * NUM_LEDS

byte leds[NUM_CHANNELS];
WS2811Driver ledStrip; // uses P1_7 as datapin connect to DIN on strip

int counter = 0;
int ihue = 0;        //-HUE (0-360)
int ibright = 0;     //-BRIGHTNESS (0-255)
int isat = 0;        //-SATURATION (0-255)

int idex = 0;        //-LED INDEX (0 to NUM_LEDS-1)
int bouncedirection = 0;  //-SWITCH FOR COLOR BOUNCE (0-1)
int red = 1;
int green = 1;
int blue = 1;

void setup(void)
{
  ledStrip.setLEDCount(NUM_LEDS); // setup for 60 leds on a strip
  ledStrip.begin();   // configure P1.7 for output
  for (byte x = 0 ; x < NUM_CHANNELS ; x++){
    leds[x] = 0;
  }
  ledStrip.write(leds);			 // write to the LEDs
  nextColor();
}

void loop() {
  color_bounceFADE(20);
}

void nextColor() {
  ihue++;
  if (ihue >= 359) {
    ihue = 0;
  }
  int thisColor[3];
  HSVtoRGB(ihue, 255, 55, thisColor);

  red = thisColor[0];
  green = thisColor[1];
  blue = thisColor[2];
  // HSVtoRGB(ihue, 255, 55, color);
}

void set_fractional_color(int index, int red, int green, int blue, int div) {
  int r1 = red;
  int g1 = green;
  int b1 = blue;
  if (r1 > 0) {
    r1 = r1/div;
  }
  if (g1 > 0) {
    g1 = g1/div;
  }
  if (b1 > 0) {
    b1 = b1/div;
  }
  set_color_led(index, r1, g1, b1);
}

void color_bounceFADE(int idelay) { //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)


  nextColor();

  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == NUM_LEDS) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  int iL1 = adjacent_cw(idex);
  int iL2 = adjacent_cw(iL1);
  int iR1 = adjacent_ccw(idex);
  int iR2 = adjacent_ccw(iR1);

  int magnitude = 100;

  for(int i = 0; i < NUM_LEDS; i++ ) {
    if (i == idex) {
      set_fractional_color(i, red, green, blue, 1);
    }
    else if (i == iL1) {
      set_fractional_color(i, red, green, blue, 4);
    }
    else if (i == iL2) {
      set_fractional_color(i, red, green, blue, 16);
    }
    else if (i == iR1) {
      set_fractional_color(i, red, green, blue, 4);
    }
    else if (i == iR2) {
      set_fractional_color(i, red, green, blue, 16);
    }
    else {
      set_color_led(i, 0, 0, 0);
    }
  }

  ledStrip.write(leds);			 // write to the LEDs
  delay(idelay);
}


//-FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
  int r;
  if (i < NUM_LEDS - 1) {
    r = i + 1;
  }
  else {
    r = 0;
  }
  return r;
}


//-FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {
    r = i - 1;
  }
  else {
    r = NUM_LEDS - 1;
  }
  return r;
}

void set_color_led(int index, int cred, int cgrn, int cblu) {  
  int offsetPos = (3*index);

  leds[(offsetPos + 0)] = cgrn;
  leds[(offsetPos + 1)] = cred;
  leds[(offsetPos + 2)] = cblu;  
}

void set_color_led(int index, int colors[3]) {
  int offsetPos = (3*index);

  leds[(offsetPos + 0)] = colors[0];
  leds[(offsetPos + 1)] = colors[1];
  leds[(offsetPos + 2)] = colors[2];  
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






