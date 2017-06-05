#include "LedStrip.h"

LedStrip::LedStrip(int pin_red, int pin_green, int pin_blue, boolean out_inverted) {
	
	pin_red_ = pin_red;
	pin_green_ = pin_green;
	pin_blue_ = pin_blue;
  inv_ = out_inverted;

  red_ = COLOR_RANGE;
  blue_ = COLOR_RANGE;
  green_ = COLOR_RANGE;
  brightness_ = 1;

  analogWriteRange(255);

  // consider possible invertion of the pins
  if(inv_){
    HI_ = LOW;
    LO_ = HIGH;
  }
  else {
    HI_ = HIGH;
    LO_ = LOW;
  }

  pinMode(pin_red_, OUTPUT);
  pinMode(pin_green_, OUTPUT);
  pinMode(pin_blue_, OUTPUT);
  
  power_ = true; //necessary for powerOff
  powerOff();
}

LedStrip::~LedStrip() {
	pinMode(pin_red_, INPUT);
	pinMode(pin_green_, INPUT);
	pinMode(pin_blue_, INPUT);
}

void LedStrip::powerOn(){
  power_ = true;
  applyColors();
}

void LedStrip::powerOff(){

  // store colors for next power-on
  int r = red_, g = green_, b = blue_;

  setColors(0, 0, 0);
  power_ = false;

  red_ = r;
  green_ = g;
  blue_ = b;
}

void LedStrip::setColors(int red, int green, int blue){

  red_ = boundaryCheck(red, 0, COLOR_RANGE);
  green_ = boundaryCheck(green, 0, COLOR_RANGE);
  blue_ = boundaryCheck(blue, 0, COLOR_RANGE);

  if(power_)
    applyColors();
}

void LedStrip::setBrightness(int brightness){
  brightness_ = (float) boundaryCheck(brightness, 0, 100)/100;
  
  if(power_)
    applyColors();
}

int LedStrip::boundaryCheck(int var, int bottom, int top){
  if(var < bottom)
    var = bottom;
  else if(var > top)
    var = top;

  return var;
}

void LedStrip::applyColors(){
  int r = red_, g = green_, b = blue_;

  r = r*brightness_;
  g = g*brightness_;
  b = b*brightness_;

  // invert output if necessary
  if(inv_){
    r = r ^ COLOR_RANGE;
    g = g ^ COLOR_RANGE;
    b = b ^ COLOR_RANGE;
  }

  analogWrite(pin_red_, r);
  analogWrite(pin_green_, g);
  analogWrite(pin_blue_, b);
}

