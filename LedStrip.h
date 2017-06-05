#ifndef LED_STRIP_H
#define LED_STRIP_H

#include "Arduino.h"

#define COLOR_RANGE 255

class LedStrip{

public:
	
	LedStrip(int pin_red, int pin_green, int pin_blue, boolean out_inverted);
	~LedStrip();

  void powerOn();   // switch strip on
  void powerOff();  // switch strip off
  // set the intensity of the different colors 
  void setColors(int red, int green, int blue);
  void setBrightness(int brightness);
	
private:

  // pins for the different colors
	int pin_red_, pin_green_, pin_blue_;
  
  
  bool power_;
  float brightness_;
  int red_, green_, blue_;  

  // marks if the output logic must be inverted
  boolean inv_;

  // high and low signals considering invertion 
  int HI_;
  int LO_;

  // fit the given value into the given boundaries
  int boundaryCheck(int var, int bottom, int top);

  // write the set colors to the output
  void applyColors();
};

#endif
