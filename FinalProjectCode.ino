int currentPosition = 0;
int currentLevel = 0;
int ledPins[] = { 33, 34, 35, 36 };
int buttonPins[] = { 32, 31, 30, 29 };
int correctPin = 37;
int incorrectPin = 38;
int sequence[8] = {};
int userPosition = 0;
int maxSequenceLength = 7;
bool lastButtonState[4] = { false, false, false, false };
bool buttonState[4] = { false, false, false, false };
bool buttonPressed = false;
int midiNotes[4] = { 59, 60, 62, 64 };

void setup() {
  randomSeed(analogRead(0));
  randomSequence();
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT);
  }
  pinMode(correctPin, OUTPUT);
  pinMode(incorrectPin, OUTPUT);
  delay(1000); // Give some time to wait for MIDI to connect
}

void loop() {
  if (currentLevel < maxSequenceLength) { // Check that the current level is less than max length of 8
    playSequence();
  } else {
    resetGame(); // If current level is max length, reset game
  }
}

void playSequence() {
  for (int i = 0; i <= currentLevel; i++) {
    digitalWrite(ledPins[sequence[i]], HIGH); // Light up LED
    usbMIDI.sendNoteOn(midiNotes[sequence[i]], 127, 1); // Play corresponding note
    delay(500); // Let LED and note play for a little bit of time
    usbMIDI.sendNoteOff(midiNotes[sequence[i]], 0, 1); // Turn off note
    digitalWrite(ledPins[sequence[i]], LOW); // Turn off LED
    delay(100);
  }
  userInput();
}

void userInput() {
  while (buttonPressed == false) { // Wait for user to press button
    int pressedButton = 0;
    for (int i = 0; i < 4; i++) {
      lastButtonState[i] = buttonState[i];
      buttonState[i] = digitalRead(buttonPins[i]);
      if (lastButtonState[i] == LOW and buttonState[i] == HIGH) { // Detect button press
        digitalWrite(ledPins[i], HIGH); // Light up corresponding LED
        usbMIDI.sendNoteOn(midiNotes[i], 127, 1); // Play corresponding MIDI note
        delay(500);
        digitalWrite(ledPins[i], LOW); // Turn off LED
        usbMIDI.sendNoteOff(midiNotes[i], 0, 1); // Turn off note
        buttonPressed = true; 
        pressedButton = i;
        if (i == sequence[userPosition]) { // Compare button press to sequence
          buttonCorrect();
        } else {
          incorrect();
        }
        delay(50);
      }
    }
  }
}

void buttonCorrect() {
  // Compare user's position to current level, advance user's position in sequence
  buttonPressed = false;
  if (userPosition == currentLevel) {
    correct();
  }
  userPosition++;
}

void correct() {
  // If button press corresponds to sequence, green LED flashes
  buttonPressed = false;
  digitalWrite(correctPin, HIGH);
  delay(500);
  digitalWrite(correctPin, LOW);
  if (currentLevel < maxSequenceLength) { // If current level is less than max of 8, advance to next level
    nextLevel();
  }
  // If user reaches max level of 8, all LEDs display at once and game resets
  if (currentLevel = maxSequenceLength) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], HIGH);
    }
    delay(1000);
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], LOW);
    }
    delay(1000);
    resetGame();
  }
}

void incorrect() {
  // If button press does not correspond to sequence, red light flashes and game resets
  buttonPressed = false;
  digitalWrite(incorrectPin, HIGH);
  delay(1000);
  digitalWrite(incorrectPin, LOW);
  delay(500);
  resetGame();
}

void nextLevel() {
  buttonPressed = false;
  userPosition = 0; // Reset the user's position to 0
  currentLevel++; // Advance level
  currentPosition = 0; // Reset the current position of sequence to 0
  playSequence();
}

void resetGame() {
  // Reset all variables to initial states
  buttonPressed = false;
  userPosition = 0;
  currentLevel = 0;
  currentPosition = 0;
  randomSequence(); // Reset sequence
  playSequence(); 
}

void randomSequence() {
  for (int i = 0; i < maxSequenceLength; i++) {
    sequence[i] = random(4); // Randomize sequence from 0-3
  }
}
