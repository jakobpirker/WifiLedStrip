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

  analogWriteRange(COLOR_RANGE);

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

void LedStrip::setColors(char* color_char)
{
  long number = (long)strtol(&color_char[1], NULL, 16);

  // Split them up into r, g, b values
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;

  this->setColors(r, g, b);
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

String LedStrip::printColorString()
{
  String ret = "#";

  // ensure, that the string is printed with 2 numbers
  ret += (String(red_, HEX).length() < 2) ? ("0" + String(red_, HEX)) : String(red_, HEX);
  ret += (String(green_, HEX).length() < 2) ? ("0" + String(green_, HEX)) : String(green_, HEX);
  ret += (String(blue_, HEX).length() < 2) ? ("0" + String(blue_, HEX)) : String(blue_, HEX);
  return ret;
}

bool LedStrip::generateJson(char* json, size_t b_size){
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  if(root.success()){
    root[JSON_BRIGHTNESS] = (int) (brightness_*100);
    root[JSON_POWER] = power_;
    root[JSON_COLOR] = this->printColorString();
    
    root.printTo(json, b_size);
    
    return true;
  }
  
  return false;
}

bool LedStrip::parseJson(char* json){
  
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);

  if(root.success()){
    if(root[JSON_BRIGHTNESS].success())
      this->setBrightness(root[JSON_BRIGHTNESS]);

    if (root[JSON_COLOR].success()) {
      char temp[JSON_BUFFER_SIZE];
      String tmpstring = root[JSON_COLOR].asString();
      tmpstring.toCharArray(temp, JSON_BUFFER_SIZE);
      this->setColors(temp);
    }

    if (root[JSON_POWER].success())
      root[JSON_POWER] ? this->powerOn() : this->powerOff();

    applySettings();
    return true;
  }
  
  return false; 
}
