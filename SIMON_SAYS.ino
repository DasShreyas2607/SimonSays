#define LED_BLUE 7
#define LED_RED 3
#define LED_RGB1 11
#define LED_RGB2 10
#define LED_RGB3 9
#define LED_GREEN 5
#define red 165
#define gre 10
#define blu 255
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64
#define SPEAKER_PIN 13
#include "pitches.h"
#define rd A0
// label main;
int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
};
//Mario main them tempo
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};


/* Constants - define pin numbers for LEDs,
   buttons and speaker, and also the game tones: */
const uint8_t ledPins[] = {LED_BLUE, LED_RED, LED_GREEN};
const uint8_t buttonPins[] = {A0, A1, A2, A3};

#define MAX_GAME_LENGTH 100

const int gameTones[] = { 5000, 3500, 2500, 5000};

/* Global variables - store the game state */
uint8_t gameSequence[MAX_GAME_LENGTH] = {0};
uint8_t gameIndex = 0;


Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
/**
   Set up the Arduino board and initialize Serial communication
*/
void setup() {
  Serial.begin(9600);
  
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }

  delay(2000);         // wait two seconds for initializing
  oled.clearDisplay(); // clear display

  oled.setTextSize(8);         
  oled.setTextColor(WHITE);    
  oled.setCursor(0, 0);       
  oled.println("MARVEL"); 
  oled.display(); 
  for (byte i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  for (byte i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(LED_RGB1, OUTPUT);
  pinMode(LED_RGB2, OUTPUT);
  pinMode(LED_RGB3, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);

  // The following line primes the random number generator.
  // It assumes pin A3 is floating (disconnected):
  randomSeed(analogRead(rd));
}

void displayScore() {
  int high = gameIndex % 100 / 10;
  int low = gameIndex % 10;
  oled.clearDisplay();
  oled.setCursor(20,0);
  if (gameIndex < 10)
  {
    oled.print("0");  
  }
  oled.println(gameIndex);
  // Serial.println(gameIndex);
  oled.display();
}

/**
   Lights the given LED and plays a suitable tone
*/
void lightLedAndPlayTone(byte ledIndex) {
  if (ledIndex == 3)
  {
    analogWrite(SPEAKER_PIN, 170);
    // Serial.println(ledIndex);
    analogWrite(LED_RGB1, gre);
    analogWrite(LED_RGB2, red);
    analogWrite(LED_RGB3, blu);
    delay(300);
    analogWrite(LED_RGB1, 0);
    analogWrite(LED_RGB2, 0);
    analogWrite(LED_RGB3, 0);
    analogWrite(SPEAKER_PIN, 0);
  }
  else
  {
    tone(SPEAKER_PIN, gameTones[ledIndex]);
    digitalWrite(ledPins[ledIndex], HIGH);
    delay(300);
    digitalWrite(ledPins[ledIndex], LOW);
    noTone(SPEAKER_PIN);
  }
}

/**
   Plays the current sequence of notes that the user has to repeat
*/
void playSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte currentLed = gameSequence[i];
    lightLedAndPlayTone(currentLed);
    delay(50);
  }
}

/**
    Waits until the user pressed one of the buttons,
    and returns the index of that button
*/
byte readButtons() {
  while (true) {
    for (byte i = 0; i < 4; i++) {
      byte buttonPin = buttonPins[i];
      if (digitalRead(buttonPin) == LOW) {
        Serial.println(buttonPin);
        return i;
      }
    }
    delay(1);
  }
}
 
void gameOver() {
  oled.clearDisplay();
  oled.setTextSize(4); 
  oled.setCursor(0, 0);       
  oled.println(" Game Over "); 
  oled.display();
  oled.setTextSize(8); 
  delay(200);
  // Serial.println(gameIndex - 1);
  gameIndex = 0;
  delay(200);
  int size = sizeof(melody) / sizeof(int);
  for(int i=0;i<3;i++)
  {
    digitalWrite(ledPins[i],HIGH);
  }
  analogWrite(LED_RGB1, gre);
  analogWrite(LED_RGB2, red);
  analogWrite(LED_RGB3, blu);
  delay(5);
  oled.setCursor(0, 0);       
  oled.println(" -- "); 
  for (int thisNote = 0; thisNote < size; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / tempo[thisNote];

    buzz(SPEAKER_PIN, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    buzz(SPEAKER_PIN, 0, noteDuration);
  }
  for(int i=0;i<3;i++)
  {
    digitalWrite(ledPins[i],LOW);
  }
  analogWrite(LED_RGB1, 0);
  analogWrite(LED_RGB2, 0);
  analogWrite(LED_RGB3, 0);
  delay(5);
}
 
void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
 
}

/**
   Get the user's input and compare it with the expected sequence.
*/
bool checkUserSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte expectedButton = gameSequence[i];
    byte actualButton = readButtons();
    lightLedAndPlayTone(actualButton);
    if (expectedButton != actualButton) {
      return false;
    }
  }

  return true;
}

/**
   Plays a hooray sound whenever the user finishes a level
*/
void playLevelUpSound() {
  tone(SPEAKER_PIN, 1000);
  delay(150);
  tone(SPEAKER_PIN, 2500);
  delay(150);
  tone(SPEAKER_PIN, 3000);
  delay(150);
  tone(SPEAKER_PIN, 3500);
  delay(150);
  tone(SPEAKER_PIN, 4000);
  delay(150);
  tone(SPEAKER_PIN, 4500);
  delay(150);
  noTone(SPEAKER_PIN);
}

/**
   The main game loop
*/
void loop() {
  displayScore();

  // Add a random color to the end of the sequence
  gameSequence[gameIndex] = random(0, 4);
  for (int i = 0; i < gameIndex-1; ++i)
  {
    int j = rand() % (gameIndex-i) + i;
    int temp = gameSequence[i];
    gameSequence[i] = gameSequence[j];
    gameSequence[j] = temp;
  }
  gameIndex++;
  if (gameIndex >= MAX_GAME_LENGTH) {
    gameIndex = MAX_GAME_LENGTH - 1;
  }

  playSequence();
  if (!checkUserSequence()) {
    gameOver();
  }

  delay(300);

  if (gameIndex > 0) {
    playLevelUpSound();
    delay(300);
  }
}
