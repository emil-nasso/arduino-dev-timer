#include <SimpleTimer.h>

#include "SevSeg.h"

#define NOT_PRESSED 1
#define SHORT_PRESS 2
#define LONG_PRESS 3
#define INTERNAL_LED_PIN 13
#define BUTTON_PIN 12
#define BUZZER_PIN 16
#define LONG_PRESS_DELAY_MS 500

// SimpleTimer: https://playground.arduino.cc/Code/SimpleTimer
// Sevseg: https://github.com/DeanIsMe/SevSeg
SevSeg sevseg; //Initiate a seven segment controller object
SimpleTimer timer;
int timerId;

int secondsLeft;

int countdownInitialValue;

long pressStarted;

boolean started = false;
boolean completed = false;
boolean longPressRegistered = false;

int buttonState = LOW;
int lastButtonState = LOW;

void setup() {

  // Serial
  Serial.begin(9600);

  // Pins
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Display setup
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 14, 15};
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  sevseg.setBrightness(90);
}

void resetTimer() {
  timer.disable(timerId);
  timer.deleteTimer(timerId);
  secondsLeft = countdownInitialValue;
  completed = false;
  started = false;
  Serial.println("resetting timer");
}

void startTimer() {
  completed = false;
  started = true;
  // countdownInitialValue = secondsLeft;
  secondsLeft = countdownInitialValue - 1;
  timerId = timer.setInterval(1000, countdown);
  Serial.println("starting timer");
}

void addMinuteToTimer() {
  countdownInitialValue += 60;
  secondsLeft = countdownInitialValue;
}


void loop() {
  // Run repetedly

  //play();
  
  int buttonPress = getButtonPress();

  if (started) {
    if (buttonPress == SHORT_PRESS) {
      resetTimer();
    }
  } else {
    if (buttonPress == LONG_PRESS) {
      addMinuteToTimer();
    }
    if (buttonPress == SHORT_PRESS && secondsLeft > 0) {
      startTimer();
    }
  }

  digitalWrite(INTERNAL_LED_PIN, buttonState);
  //tone(BUZZER_PIN, 100);
  timer.run();
  updateDisplay();
  sevseg.refreshDisplay();
}


int getButtonPress() {
  buttonState = digitalRead(BUTTON_PIN);
  boolean buttonWasShortPressed = false;
  boolean buttonWasLongPressed = false;

  if (buttonState == HIGH) {
    if (buttonState != lastButtonState) {
      pressStarted = millis();
      longPressRegistered = false;
    }
    if (millis() - pressStarted > LONG_PRESS_DELAY_MS) {
      longPressRegistered = true;
      buttonWasLongPressed = true;
      pressStarted = millis();
    }
  }
  if (buttonState == LOW && buttonState != lastButtonState) {
    if (longPressRegistered == false) {
      buttonWasShortPressed = true;
    }
    longPressRegistered = false;
  }
  lastButtonState = buttonState;

  if (buttonWasLongPressed) {
    Serial.println("LongPress");
    return LONG_PRESS;
  }
  if (buttonWasShortPressed) {
    Serial.println("ShortPress");
    return SHORT_PRESS;
  }
  return NOT_PRESSED;
}


void countdown() {
  // Serial.print("Uptime (s): ");
  // Serial.println(millis() / 1000);
  if (started) {
    secondsLeft--;
  }

  if (secondsLeft == 0) {
    completed = true;
    timer.disable(timerId);
  }

}

void updateDisplay() {
  if (completed) {
    sevseg.setChars("byta");
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
    int minutes = secondsLeft / 60;
    int seconds = secondsLeft % 60;
    sevseg.setNumber(minutes * 100 + seconds, 2);
  }

}
