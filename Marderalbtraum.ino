#include "OXOcardRunner.h"
#include <StateMachine.h>

// Marderalbtraum SW
// Version 1.0
// Copyright 2021, Marc Singer, BBGS21.f

// For reference, look at the full source code of the oxocard (https://github.com/oxon)
// No promises you will find anything useful there thought

// Refer to README.txt to learn how to compile this

const int STATE_TRANSITION_DELAY = 100; // Time in ms to switch between states
const int MARDER_DETECTION_RATE = 1000; // Time in ms between movement detection checks
const int BATTERY_RECOVERY_CHECK_RATE = 1000; // Time in ms between battery recovery checks in s3
const float BATTERY_MIN_VOLTAGE = 3.4; // Minimal voltage of the battery to fall into s3
const int MARDER_DETECTION_PORT = 18;  // GPIO Port for Marder detection (ext. motion sensor signal port)
const int MARDER_FRIGHT_TIME = 10; // Time a marder is frightened in s
const int MARDER_FRIGHT_START_FREQ = 13000; // Sound output starts at this freq in khz
const int MARDER_FRIGHT_END_FREQ = 18000; // Sound output ends at this freq in khz
const int STATUS_LED_ROW = 7; // Row of the status LED in the LED matrix
const int STATUS_LED_COL = 7; // Column of the status LED in the LED matrix

// Creates and handles the StateMachine
// Supports transitions between states
// External library (source can be provided)
StateMachine state = StateMachine();

// State reference table
// Maps states to functions that are called when a specific state is reached
// The condition to reach each state is evaluated by bool returning functions and setup in the setup function
State* s0 = state.addState(&stateDetection); // 0: Marder detection (default state)
State* s1 = state.addState(&stateFright); // 1: Marder frightening
State* s2 = state.addState(&stateLowBattery); // 2: Battery low
State* s3 = state.addState(&stateShutdown); // 3: Shutdown system

// Marder Detection function
// Evaluates if a marder has been detected by querying the external sensor
bool detectMarder() {
  if (digitalRead(MARDER_DETECTION_PORT)==HIGH) {
    return true;
  }
  return false;
}

// Low Battery detection function
// Evaluates if the battery of the oxooxocard is to low to properly function
bool detectLowBattery() {
  if (oxocard.battery->getVoltage() < BATTERY_MIN_VOLTAGE) {
    return true;
  }
  return false;
}

// After a marder has been frightened, return to detection state s0
// This could be enhanced to do other checks, like listen to microphone, to check if the marder has truly ran away
bool detectMarderFrightened() {
  return true;
}

// Detect 2 simultanious button presses to trigger shutdown mode
// Button 5 & 6 need to be pressed simultaniously
// Oxocard maps these buttions to R (right) and L (left), L2 & L3 are also hardcoded to reset
bool detectShutdown() {
  if (oxocard.button->isPressed(R2) && oxocard.button->isPressed(R3)) {
    return true;
  }
  return false;
}

// Detection state
// Awaiting motion detection, while only cleaning the screen once and showing transition in debug
void stateDetection() {
  if (state.executeOnce) {
    // Clean screen
    cleanScreen();
    // Show in debugger that we entered this state
    Serial.println("Switched to S0");
    Serial.println("Awaiting motion detection");
  }
  setStatusOK(); // Show green status light
  delay(MARDER_DETECTION_RATE); // Await state change so battery does not drain as fast
}

// Marder fright state
// Uses connected devices (LEDs & speaker) to fright the marder
void stateFright() {
  if (state.executeOnce) {
    // Show in debugger that we entered this state
    Serial.println("Switched to S1");
    Serial.println("Motion has been detected");
  }
  // Hold current amount of seconds
  int i;
  // Hold current frequency for sound
  int freq = MARDER_FRIGHT_START_FREQ;
  // Calculate incrementation steps
  int freq_incr = (MARDER_FRIGHT_END_FREQ - MARDER_FRIGHT_START_FREQ) / MARDER_FRIGHT_TIME;
  // Loop until time has finished
  for (i = 0; i < MARDER_FRIGHT_TIME; i++) {
    // Enable audio
    oxocard.audio->tone(freq);
    // Clean the screen and wait 500ms
    cleanScreen();
    delay(500);
    // Blink screen for 500ms
    blinkScreen();
    delay(500);
    // Disable audio
    oxocard.audio->noTone();
    // Increment frequency
    freq = freq + freq_incr;
  }
}

// Low battery state
// This shows low battery to the user
// Checks for battery charging itself and switches back the state to detection once it is charged
void stateLowBattery() {
  if (state.executeOnce) {
    // Clean screen
    cleanScreen();
    // Show in debugger that we entered this state
    Serial.println("Switched to S2");
    Serial.println("Enable automated shutdown");
    // Enable magic shutdown
    // This should shutdown in 30 seconds (plot twist: it doesn't)
    AutoTurnOff::getInstance().enable();
  }
  setStatusBatteryLow();  // Change status LED to red
  if (!detectLowBattery()) {
    // Transition to S0 (Detection state) once battery is charged enough
    // This has to happen before system auto shutdown (good luck)
    AutoTurnOff::getInstance().disable();
    state.transitionTo(s0);
  }
  delay(BATTERY_RECOVERY_CHECK_RATE);  // Await state change so battery does not drain as fast
}

// Shutdown state
// Allows to shutdown the device without any magic 30 second autoshutdown bs
void stateShutdown() {
  if (state.executeOnce) {
    // Clean screen
    cleanScreen();
    // Show in debugger that we entered this state
    Serial.println("Switched to S4");
    Serial.println("Shutting down system");
    // Turn off the oxocard
    oxocard.system->turnOff();
  }
}

// Device Setup
void setup() {
  // Set Motion Detection IO PIN to read mode
  // This is absolutly redundant, but with oxocard you never know
  pinMode(MARDER_DETECTION_PORT, INPUT);
  // Disable magic turnoff after 30? seconds
  AutoTurnOff::getInstance().disable();
  // Setup serial debugger with a good baud rate
  Serial.begin(115200);
  // Clean screen
  cleanScreen();
  // Add a transition to marder frightening state if a marder has been detected
  s0->addTransition(&detectMarder, s1);
  // After a marder has been frightened, return back to s0
  s1->addTransition(&detectMarderFrightened, s0);
  // Add all low battery states, detection is halted if this state has been detected
  // State forces to go back to s0 once battery has been charged in &stateLowBattery()
  s0->addTransition(&detectLowBattery, s2);
  s1->addTransition(&detectLowBattery, s2);
  // Add shutdown to all states
  s0->addTransition(&detectShutdown, s3);
  s1->addTransition(&detectShutdown, s3);
  s2->addTransition(&detectShutdown, s3);
}

// Main loop
void loop() {
  state.run(); // Run the state machine
  delay(STATE_TRANSITION_DELAY); // Wait transition delay in between switching states
}

// Clears the LED matrix of the oxocard
// This is just a wrapper so we don't have to write oxocard.matrix everytime
void cleanScreen() {
  oxocard.matrix->clearScreen();
}

// Forces update of the LED matrix of the oxocard
// Not sure if this is actually needed, since the oxocard uses black magic single buffering
void updateScreen() {
  if (oxocard.matrix->isAutoUpdate()) {
    oxocard.matrix->update();
  }
}

// Blinks the LED matrix of the Oxocard in white color
void blinkScreen() {
  oxocard.matrix->fillScreen(rgb(255,255,255));
}

// Sets the status led to OK (RGB Green)
void setStatusOK() {
  oxocard.matrix->setPixel(STATUS_LED_ROW,STATUS_LED_COL, rgb(0,255,8));
  updateScreen();
}

// Sets the status led to BatteryLow (RGB Red)
void setStatusBatteryLow() {
  oxocard.matrix->setPixel(STATUS_LED_ROW,STATUS_LED_COL, rgb(255,40,0));
  updateScreen();
}
