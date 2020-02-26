/*

StairsLight
This project is build to add some light under the steps on my stairs.
https://www.w3schools.com/cpp/cpp_constructors.asp
*/

#include <Arduino.h>                  // Default library to use an external IDE
#define FASTLED_INTERNAL              // Remove FastLED compilation Warnings
#include <FastLED.h>                  // Include FastLED Library
#define NUM_LEDS 36                   // Total number of LEDs
#define NUM_STEPS 3                   // Steps number per staircase
#define LEDS_PER_STEPS 6              // LED per step
#define FRAMES_PER_SECOND 60          // Frame per second
#define DEFAULT_BRIGHTNESS 60         // Default Brightness level 0-255
#define LED_STRIP_VOLTAGE 5           // LED strip working voltage ( Usually 5V )
#define LED_STRIP_MILLIAMPS 400       // Max current to draw ( in mA )
#define DATA_PIN 53                   // LED strip Data PIN
#define FIRST_STAIR_BOT_SENSOR_PIN 2  // Bottom staircase downstairs Sensor
#define FIRST_STAIR_TOP_SENSOR_PIN 3  // Bottom staircase upstairs Sensor
#define SECOND_STAIR_BOT_SENSOR_PIN 4 // Uppper staircase downstairs Sensor
#define SECOND_STAIR_TOP_SENSOR_PIN 5 // Uppper staircase upstairs Sensor

/*
// Setup LEDS Array
*/

// CRGB leds[NUM_LEDS];   // Not using it
CRGBArray<NUM_LEDS> leds;                                  // Create FastLED objecty
CRGBSet firstStairLeds(leds(0, (NUM_LEDS / 2) - 1));       // Create downStairLed sub Array
CRGBSet secondStairLeds(leds(NUM_LEDS / 2, NUM_LEDS - 1)); // Create upStairLed sub Array

/*
// loop global variables below
*/

long firstStairTimeout = 0;          //  Timestamp to remember when the PIR was triggered on the first stair.
long secondStairTimeout = 0;         //  Timestamp to remember when the PIR was triggered on the second stair.
uint8_t firstStairAction = 0;        //  0: default ; 1: lit from bot to top ; 2: lit from top to bot ; 3: wipe from bot to top ; 4: wipe from top to bot
uint8_t secondStairAction = 0;       //  0: default ; 1: lit from bot to top ; 2: lit from top to bot ; 3: wipe from bot to top ; 4: wipe from top to bot
bool firstStairBotSensorStatus = 0;  // 1st stair bot PIR sensor
bool firstStairTopSensorStatus = 0;  // 1st stair top PIR sensor
bool secondStairBotSensorStatus = 0; // 2nd stair bot PIR sensor
bool secondStairTopSensorStatus = 0; // 2nd stair top PIR sensor

uint16_t firstStairLedPointer = 0;  // 1st stair function LED variable
uint16_t secondStairLedPointer = 0; // 2nd stair function LED variable

uint8_t firstStairStepPointer = 0;  // 1st stair function Steps variable
uint8_t secondStairStepPointer = 0; // 2nd stair function Steps variable

/* 
// C++ functions definiton  
*/

// Set actions regarding stair sensors
void setActions(bool botSensor, bool topSensor, uint8_t *action, long *timeout);

/*
// Main loops
*/

void setup()
{
        delay(2000);           // Startup delay
        Serial.begin(9600);    // Setup Serial console for Debug purpose
        pinMode(13, OUTPUT);   // Setup LED 13 to Stop it
        digitalWrite(13, LOW); // Stop LED 13

        // Setup sensors
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
        firstStairLeds[0] = CRGB::Red;
        FastLED.show();
        delay(200);
        firstStairLeds[0] = CRGB::Green;
        FastLED.show();
        delay(200);
        firstStairLeds[0] = CRGB::Blue;
        FastLED.show();
        delay(200);
        firstStairLeds[0] = CHSV(0, 0, 255); // Blanc
        FastLED.show();
        delay(200);
        FastLED.clear();
        FastLED.show();
}

void loop()
{
        // Check sensors status. INPUT_PULLUP, set the opposite value
        firstStairBotSensorStatus = !digitalRead(FIRST_STAIR_BOT_SENSOR_PIN);
        firstStairTopSensorStatus = !digitalRead(FIRST_STAIR_TOP_SENSOR_PIN);
        secondStairBotSensorStatus = !digitalRead(SECOND_STAIR_BOT_SENSOR_PIN);
        secondStairTopSensorStatus = !digitalRead(SECOND_STAIR_TOP_SENSOR_PIN);

        // Set actions on stairs regarding the sensors
        setActions(firstStairBotSensorStatus, firstStairTopSensorStatus, &firstStairAction, &firstStairTimeout);
        setActions(secondStairBotSensorStatus, secondStairTopSensorStatus, &secondStairAction, &secondStairTimeout);

        // Set animations regarding the actions value

        // Default Animations
        if (!firstStairBotSensorStatus && !firstStairTopSensorStatus && firstStairAction == 0)
        {
                // Reset animation variables
                firstStairTimeout = 0;
                firstStairLedPointer = 0;
                firstStairStepPointer = 0;
                // Waiting animation
                float breath = (exp(sin(millis() / 2000.0 * PI)) - 0.36787944) * 108.0;
                breath = map(breath, 0, 255, 64, 128);
                for (uint16_t s = 0; s < NUM_STEPS; s++)
                { // Light up 1 leds on the right and on the left on each steps
                        int shift = s * LEDS_PER_STEPS;
                        firstStairLeds(shift, shift) = CHSV(160, 255, breath); // Blue
                        firstStairLeds(shift + LEDS_PER_STEPS - 1, shift + LEDS_PER_STEPS - 1) = firstStairLeds(shift, shift);
                }
        }
        if (!secondStairBotSensorStatus && !secondStairTopSensorStatus && secondStairAction == 0)
        {
                // Reset animation variables
                secondStairTimeout = 0;
                secondStairLedPointer = 0;
                secondStairStepPointer = 0;
                // Waiting animation
                float breath = (exp(sin(millis() / 2000.0 * PI)) - 0.36787944) * 108.0;
                breath = map(breath, 0, 255, 64, 128);
                for (uint16_t s = 0; s < NUM_STEPS; s++)
                { // Light up 1 leds on the right and on the left on each steps
                        int shift = s * LEDS_PER_STEPS;
                        secondStairLeds(shift, shift) = CHSV(160, 255, breath); // Blue
                        secondStairLeds(shift + LEDS_PER_STEPS - 1, shift + LEDS_PER_STEPS - 1) = secondStairLeds(shift, shift);
                }
        }

        // First Stair Animations

        switch (firstStairAction)
        {
        case 1:                            // Steps animation from bottom to top
                firstStairStepPointer = 0; // Reset stepPointer if wipe is not finished and the sensor reactivated
                EVERY_N_MILLISECONDS(5)
                {
                        firstStairLeds(0, firstStairLedPointer) = CHSV(0, 0, 255); // White
                        if (int16_t(firstStairLedPointer) < firstStairLeds.size() - 1)
                        {
                                firstStairLedPointer++;
                        }
                }
                break;

        case 2:                            // Steps animation from top to bottom
                firstStairStepPointer = 0; // Reset stepPointer if wipe is not finished and the sensor reactivated
                EVERY_N_MILLISECONDS(5)
                {
                        firstStairLeds(firstStairLeds.size() - 1, firstStairLeds.size() - 1 - firstStairLedPointer) = CHSV(0, 0, 255); // White
                        if (int16_t(firstStairLedPointer) < firstStairLeds.size() - 1)
                        {
                                firstStairLedPointer++;
                        }
                }
                break;

        case 3: // wipe steps from bottom to top
                EVERY_N_MILLISECONDS(3000 / NUM_STEPS)
                {
                        if (firstStairStepPointer < NUM_STEPS)
                        {
                                firstStairLeds(0, firstStairStepPointer * LEDS_PER_STEPS + LEDS_PER_STEPS - 1) = CHSV(0, 0, 0); // Black
                                firstStairStepPointer++;
                        }
                        else
                        {
                                firstStairAction = 0;
                        }
                }
                break;

        case 4: // wipe steps from top to bottom
                EVERY_N_MILLISECONDS(3000 / NUM_STEPS)
                {
                        if (firstStairStepPointer < NUM_STEPS)
                        {
                                firstStairLeds(firstStairLeds.size() - 1, (NUM_STEPS - firstStairStepPointer) * LEDS_PER_STEPS - LEDS_PER_STEPS) = CHSV(0, 0, 0); // Black
                                firstStairStepPointer++;
                        }
                        else
                        {
                                firstStairAction = 0;
                        }
                }
                break;

        default:
                break;
        }

        // Second Stair Animations

        switch (secondStairAction)
        {
        case 1:                             // Steps animation from bottom to top
                secondStairStepPointer = 0; // Reset stepPointer if wipe is not finished and the sensor reactivated
                EVERY_N_MILLISECONDS(5)
                {
                        secondStairLeds(0, secondStairLedPointer) = CHSV(0, 0, 255); // White
                        if (int16_t(secondStairLedPointer) < secondStairLeds.size() - 1)
                        {
                                secondStairLedPointer++;
                        }
                }
                break;

        case 2:                             // Steps animation from top to bottom
                secondStairStepPointer = 0; // Reset stepPointer if wipe is not finished and the sensor reactivated
                EVERY_N_MILLISECONDS(5)
                {
                        secondStairLeds(secondStairLeds.size() - 1, secondStairLeds.size() - 1 - secondStairLedPointer) = CHSV(0, 0, 255); // White
                        if (int16_t(secondStairLedPointer) < secondStairLeds.size() - 1)
                        {
                                secondStairLedPointer++;
                        }
                }
                break;

        case 3: // wipe steps from bottom to top
                EVERY_N_MILLISECONDS(3000 / NUM_STEPS)
                {
                        if (secondStairStepPointer < NUM_STEPS)
                        {
                                secondStairLeds(0, secondStairStepPointer * LEDS_PER_STEPS + LEDS_PER_STEPS - 1) = CHSV(0, 0, 0); // Black
                                secondStairStepPointer++;
                        }
                        else
                        {
                                secondStairAction = 0;
                        }
                }
                break;

        case 4: // wipe steps from top to bottom
                EVERY_N_MILLISECONDS(3000 / NUM_STEPS)
                {
                        if (secondStairStepPointer < NUM_STEPS)
                        {
                                secondStairLeds(secondStairLeds.size() - 1, (NUM_STEPS - secondStairStepPointer) * LEDS_PER_STEPS - LEDS_PER_STEPS) = CHSV(0, 0, 0); // Black
                                secondStairStepPointer++;
                        }
                        else
                        {
                                secondStairAction = 0;
                        }
                }
                break;

        default:
                break;
        }

        FastLED.show();
}

void setActions(bool botSensor, bool topSensor, uint8_t *action, long *timeout)
{
        if (botSensor && *action != 2)
        {
                *action = 1;
                *timeout = millis();
        }
        if (topSensor && *action != 1)
        {
                *action = 2;
                *timeout = millis();
        }
        if (*action == 1)
        {
                if (millis() - *timeout > 10000)
                {
                        *action = 3;
                }
        }
        if (*action == 2)
        {
                if (millis() - *timeout > 10000)
                {
                        *action = 4;
                }
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
