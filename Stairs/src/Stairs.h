/*
  Stairs.h - Library for controlling any stairs LEDs.
  Created by Alexandre Pinon, September 2019.
  Released into the public domain.
*/

#ifndef Stairs_h
#define Stairs_h

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

class Stairs
{
public:
  Stairs(CRGBSet *ledStrip, uint8_t numSteps, const uint8_t lowerSensorPin, const uint8_t upperSensorPin);
  bool getLowerSensorStatus();
  bool getUpperSensorStatus();
  uint8_t getAction();
  void updateSensorsStatus();
  void updateActions();
  void updateAnimations();
  

//private:
  CRGBSet m_ledStrip;
  uint8_t m_numSteps;
  uint16_t m_numLeds;
  uint8_t m_ledsPerStep;
  uint8_t m_lowerSensorPin;
  uint8_t m_upperSensorPin;
  bool m_lowerSensorStatus;
  bool m_upperSensorStatus;
  long m_timeout;
  uint8_t m_action;
  uint16_t m_ledPointer;
  uint8_t m_stepPointer;
};

#endif