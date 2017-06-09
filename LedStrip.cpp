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
  applySettings();
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
  applySettings();
}

void LedStrip::setBrightness(int brightness){
  brightness_ = (float) boundaryCheck(brightness, 0, 100)/100;
  applySettings();
}

int LedStrip::boundaryCheck(int var, int bottom, int top){
  if(var < bottom)
    var = bottom;
  else if(var > top)
    var = top;

  return var;
}

void LedStrip::applySettings(){
  
  if(power_){
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
}

bool LedStrip::generateJson(char* json, size_t b_size){
  JsonObject& root = jsonBuffer_.createObject();
  
  if(root.success()){
    root[JSON_BRIGHTNESS] = brightness_;
    root[JSON_POWER] = (int) power_;
    root[JSON_RED] = red_;
    root[JSON_GREEN] = green_;
    root[JSON_BLUE] = blue_;
    
    root.printTo(json, b_size);
    
    return true;
  }
  
  // return root.printTo(char*);
  return false;
}

bool LedStrip::parseJson(char* json){
  
  JsonObject& root = jsonBuffer_.parseObject(json);
  
  if(root.success()){
    brightness_ = root[JSON_BRIGHTNESS].success() ? root[JSON_BRIGHTNESS] : brightness_;
    power_ = root[JSON_POWER].success() ? (bool) root[JSON_POWER] : power_;
    red_ = root[JSON_RED].success() ? root[JSON_RED] : red_;
    green_ = root[JSON_GREEN].success() ? root[JSON_GREEN] : green_;
    blue_ = root[JSON_BLUE].success() ? root[JSON_BLUE] : blue_;
    
    applySettings();
    return true;
  }
  
  return false;
 
}
