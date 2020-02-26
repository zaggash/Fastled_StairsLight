/*

StairsLight
This project is build to add some light under the steps on my stairs.

*/

#include <Arduino.h>                  // Default library to use an external IDE
#define FASTLED_INTERNAL              // Remove FastLED compilation Warnings
#include <FastLED.h>                  // Include FastLED Library
#include "Stairs.h"                   // Include local Stairs Library
#define NUM_LEDS 36                   // Total number of LEDs
#define NUM_STEPS 2                   // Steps number per staircase
//#define LEDS_PER_STEPS 18             // LED per step
#define FRAMES_PER_SECOND 60         // Frame per second
#define DEFAULT_BRIGHTNESS 60         // Default Brightness level 0-255
#define LED_STRIP_VOLTAGE 5           // LED strip working voltage ( Usually 5V )
#define LED_STRIP_MILLIAMPS 400       // Max current to draw ( in mA )
#define DATA_PIN 53                   // LED strip Data PIN
#define FIRST_STAIR_BOT_SENSOR_PIN 2  // Bottom staircase downstairs Sensor
#define FIRST_STAIR_TOP_SENSOR_PIN 3  // Bottom staircase upstairs Sensor
#define SECOND_STAIR_BOT_SENSOR_PIN 4 // Uppper staircase downstairs Sensor
#define SECOND_STAIR_TOP_SENSOR_PIN 5 // Uppper staircase upstairs Sensor

/*
// Global variables
*/

CRGBArray<NUM_LEDS> leds;                                  // Create FastLED objecty
CRGBSet firstStairLeds(leds(0, (NUM_LEDS / 2) - 1));       // Create downStairLed sub Array
CRGBSet secondStairLeds(leds(NUM_LEDS / 2, NUM_LEDS - 1)); // Create upStairLed sub Array
Stairs lowerStair(& firstStairLeds, NUM_STEPS, FIRST_STAIR_BOT_SENSOR_PIN, FIRST_STAIR_TOP_SENSOR_PIN);
Stairs upperStair(& secondStairLeds, NUM_STEPS, SECOND_STAIR_BOT_SENSOR_PIN, SECOND_STAIR_TOP_SENSOR_PIN);

/*
// Main loops
*/

void setup()
{
        delay(2000);           // Startup delay
        Serial.begin(9600);    // Setup Serial console for Debug purpose
        pinMode(13, OUTPUT);   // Setup LED 13 to Stop it
        digitalWrite(13, LOW); // Stop LED 13

        pinMode(FIRST_STAIR_BOT_SENSOR_PIN, INPUT_PULLUP);
        pinMode(FIRST_STAIR_TOP_SENSOR_PIN, INPUT_PULLUP);
        pinMode(SECOND_STAIR_BOT_SENSOR_PIN, INPUT_PULLUP);
        pinMode(SECOND_STAIR_TOP_SENSOR_PIN, INPUT_PULLUP);

        // Setup FastLED State
        FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);                            // Add LEDs to the object
        FastLED.setCorrection(TypicalSMD5050);                                          // Set Color Correction
        FastLED.setTemperature(DirectSunlight);                                         // Set Temperature Correction
        FastLED.setMaxPowerInVoltsAndMilliamps(LED_STRIP_VOLTAGE, LED_STRIP_MILLIAMPS); // FastLED Max Power usage
        set_max_power_indicator_LED(13);                                                // Use integrated LED to show if we reach Max Power
        FastLED.setBrightness(DEFAULT_BRIGHTNESS);                                      //Set default brightness
        FastLED.clear();                                                                // Clear the strips from any colors
        FastLED.show();

        // Start a quick LED test
        leds[0] = CRGB::Red;
        FastLED.show();
        delay(200);
        leds[0] = CRGB::Green;
        FastLED.show();
        delay(200);
        leds[0] = CRGB::Blue;
        FastLED.show();
        delay(200);
        leds[0] = CHSV(0, 0, 255); // Blanc
        FastLED.show();
        delay(200);
        FastLED.clear();
        FastLED.show();
}

void loop()
{
        // Check sensors status.
        EVERY_N_MILLISECONDS(500) {
        lowerStair.updateSensorsStatus();
        upperStair.updateSensorsStatus();
        }

        EVERY_N_MILLISECONDS(500) {
        // Set actions on stairs regarding the sensors
        lowerStair.updateActions();
        upperStair.updateActions();
        }

        // Set animations regarding the actions value
        lowerStair.updateAnimations();
        upperStair.updateAnimations();
        

        EVERY_N_MILLISECONDS(1000 / FRAMES_PER_SECOND) // insert a delay to keep the framerate modest
        {
                FastLED.show();
        }
}

//void mirroredAnimation(uint32_t t) { // t Time in ms between each led to light up
//  bool odd = NUM_LEDS % 2; // If the LEDs number is odd then odd is 1
//  if (odd) {
//    leds[(NUM_LEDS - odd) / 2] = CRGB::White; // If odd number, then light up the middle LED
//    FastLED.show();
//    delay(t);
//  }
//  for (uint32_t i = ((NUM_LEDS - odd) / 2); i < (NUM_LEDS - odd); i++ ) {
//    leds[i + odd] = CRGB::White;
//    leds[NUM_LEDS - odd - (i + 1) ] = CRGB::White;
//    FastLED.show();
//    delay(t);
//  }
//}
