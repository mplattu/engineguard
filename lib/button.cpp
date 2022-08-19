int buttonPin;
unsigned long buttonLastChange = 0;
unsigned long buttonPressValidityTime; // Seconds
volatile bool buttonPressedSinceLastCycle;

void IRAM_ATTR buttonIsPressed() {
// https://stackoverflow.com/questions/71992044/esp32-core-1-paniced-interrupt-wdt-timeout-on-cpu1
//  Serial.print("buttonIsPressed\n");
  buttonPressedSinceLastCycle = true;
}

unsigned long buttonGetTime() {
  return (millis() / 1000);
}


bool buttonPressIsStillValid() {
  unsigned long now = buttonGetTime();

  if ((buttonLastChange + buttonPressValidityTime) < now) {
    return false;
  }

  return true;
}

void buttonSetup(int paramButtonPin, unsigned long paramButtonPressValidityTime) {
  buttonPressValidityTime = paramButtonPressValidityTime;

  buttonPin = paramButtonPin;
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonIsPressed, CHANGE);
}

void buttonLoop() {
  if (buttonPressedSinceLastCycle) {
    buttonPressedSinceLastCycle = false;
    buttonLastChange = buttonGetTime();
  }
}
