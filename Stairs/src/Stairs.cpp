/*
  Stairs.cpp - Library for controlling any stairs LEDs.
  Created by Alexandre Pinon, September 2019.
  Released into the public domain.
*/

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "Stairs.h"

Stairs::Stairs(CRGBSet *ledStrip, uint8_t numSteps, const uint8_t lowerSensorPin, const uint8_t upperSensorPin) : m_ledStrip(*ledStrip)
{
  // Set deafult object attribtues
  m_numSteps = numSteps;
  m_numLeds = m_ledStrip.size();
  m_ledsPerStep = m_numLeds / m_numSteps;
  m_lowerSensorPin = lowerSensorPin;
  m_upperSensorPin = upperSensorPin;
  m_lowerSensorStatus = !digitalRead(m_lowerSensorPin);
  m_upperSensorStatus = !digitalRead(m_upperSensorPin);
  m_timeout = 0;
  m_action = 0;
  m_ledPointer = 0;
  m_stepPointer = 0;
}

bool Stairs::getLowerSensorStatus()
{
  return m_lowerSensorStatus;
}
bool Stairs::getUpperSensorStatus()
{
  return m_upperSensorStatus;
}

uint8_t Stairs::getAction()
{
  return m_action;
}

void Stairs::updateSensorsStatus()
{
  m_lowerSensorStatus = !digitalRead(m_lowerSensorPin);
  m_upperSensorStatus = !digitalRead(m_upperSensorPin);
}

void Stairs::updateActions()
{
  if (m_lowerSensorStatus && m_action != 2)
  {
    m_action = 1;
    m_timeout = millis();
  }
  if (m_upperSensorStatus && m_action != 1)
  {
    m_action = 2;
    m_timeout = millis();
  }
  if (m_action == 1)
  {
    if (millis() - m_timeout > 10000)
    {
      m_action = 3;
    }
  }
  if (m_action == 2)
  {
    if (millis() - m_timeout > 10000)
    {
      m_action = 4;
    }
  }
}

void Stairs::updateAnimations()
{
  switch (m_action)
  {
  case 0:
    if (!m_lowerSensorStatus && !m_upperSensorStatus)
    {
      // Reset animation variables
      m_timeout = 0;
      m_ledPointer = 0;
      m_stepPointer = 0;
      // Waiting animation
      float breath = (exp(sin(millis() / 2000.0 * PI)) - 0.36787944) * 108.0;
      breath = map(breath, 0, 255, 64, 128);
      for (uint16_t s = 0; s < m_numSteps; s++)
      { // Light up 2 leds on the right and on the left on each steps
        int shift = s * m_ledsPerStep;
        m_ledStrip(shift, shift) = CHSV(160, 255, breath); // Blue
        m_ledStrip(shift + m_ledsPerStep - 1, shift + m_ledsPerStep - 1) = m_ledStrip(shift, shift);
      }
    }
    break;

  case 1:
    m_stepPointer = 0;
    EVERY_N_MILLISECONDS(3000 / m_numLeds)
    {
      m_ledStrip[m_ledPointer] = CHSV(0, 0, 255); // White
      if (int16_t(m_ledPointer) < int16_t(m_numLeds) - 1)
      {
        m_ledPointer++;
      }
    }
    break;

  case 2:
    m_stepPointer = 0;
    EVERY_N_MILLISECONDS(3000 / m_numLeds)
    {
      m_ledStrip[m_numLeds - 1 - m_ledPointer] = CHSV(0, 0, 255); // White
      if (int16_t(m_ledPointer) < int16_t(m_numLeds) - 1)
      {
        m_ledPointer++;
      }
    }
    break;

  case 3:
    EVERY_N_MILLISECONDS(3000 / m_numSteps)
    {
      m_ledStrip(0, m_stepPointer * m_ledsPerStep + m_ledsPerStep - 1) = CHSV(0, 0, 0); // Black
      if (m_stepPointer < m_numSteps - 1)
      {
        m_stepPointer++;
      }
      else
      {
        m_action = 0;
      }
    }
    break;

  case 4:
    EVERY_N_MILLISECONDS(3000 / m_numSteps)
    {
      m_ledStrip(m_numSteps * m_ledsPerStep - 1, (m_numSteps - m_stepPointer) * m_ledsPerStep - m_ledsPerStep) = CHSV(0, 0, 0); // Black
      if (m_stepPointer < m_numSteps - 1)
      {
        m_stepPointer++;
      }
      else
      {
        m_action = 0;
      }
    }
    break;

  default:
    break;
  }
}