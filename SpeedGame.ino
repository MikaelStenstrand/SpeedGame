#include "TimerObject.h"

#define PLAY_STACK_LENGTH 3
#define LED_AMOUNT 4
#define DEFAULT_PLAY_SEQUENCE 2000

#define DEBUG 2

#define LED_GREEN 2
#define LED_BLUE  3
#define LED_WHITE 4
#define LED_RED   5

#define DELAY_OF_BUTTON_DEBOUNCE 100
#define LED_SEQUENCE_DECREASE_INTERVAL_FACTOR 0.95
#define LED_TURN_OFF_SEQUENCE_FACTOR 0.60

// BUTTONS
#define BUTTON_GREEN  9
#define BUTTON_BLUE	  10
#define BUTTON_WHITE  11
#define BUTTON_RED	  12

const int leds[] = {
  LED_GREEN,
  LED_BLUE,
  LED_WHITE,
  LED_RED,
};

const int buttons[] = {
  BUTTON_GREEN,
  BUTTON_BLUE,
  BUTTON_WHITE,
  BUTTON_RED,
};


int buttonGreenState 	= 0;
int buttonBlueState		= 0;
int buttonWhiteState	= 0;
int buttonRedState		= 0;

long timeOfLastButtonDebounce = 0;

// PLAY LOGIC
int playStack[PLAY_STACK_LENGTH];
int playStackIndex = 0;
int playerScore = 0;
int randomIndex;

int ledSequenceInterval = DEFAULT_PLAY_SEQUENCE;
bool isGameOver = false;

TimerObject *timerLedSequence = new TimerObject(ledSequenceInterval);
TimerObject *timerLedTurnOffSequence = new TimerObject((int)(ledSequenceInterval * LED_TURN_OFF_SEQUENCE_FACTOR));

void setup() {
  Serial.begin(9200);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  pinMode(BUTTON_GREEN, INPUT);
  pinMode(BUTTON_BLUE, INPUT);
  pinMode(BUTTON_WHITE, INPUT);
  pinMode(BUTTON_RED, INPUT);

  startupLedBlinking();

  randomSeed(analogRead(0));

  updateDisplayScore();
  resetPlayStack();

  // TIMERS
  timerLedSequence->setOnTimer(&playLedSequence);
  timerLedSequence->Start();

  timerLedTurnOffSequence->setOnTimer(&ledTurnOffSequence);
  timerLedTurnOffSequence->setSingleShot(true);
  timerLedTurnOffSequence->Start();
}


void loop() {
  checkForInputs();
  if (timerLedSequence->isEnabled())
    timerLedSequence->Update();
  if (timerLedTurnOffSequence->isEnabled())
    timerLedTurnOffSequence->Update();
}

// lid random led and add it to the play sequence array
// called by timer
void playLedSequence()	{
  if (DEBUG == 1) {
    Serial.println("--- PLAY LED SEQUENCE ---");
    Serial.println("INTERVAL: " + (String)ledSequenceInterval);
  }
  randomIndex = getRandomLedIndex();
  bool gameNotOver = addToPlayStack(randomIndex);

  if (gameNotOver)	{
    lidLed(randomIndex);
    ledSequenceInterval = decreaseInterval(ledSequenceInterval);
    updateLedTimer(ledSequenceInterval);
    updateLedTurnOffTimer(ledSequenceInterval);
  } else	{
  	gameOver();
  }
}

int getRandomLedIndex() {
  return random(0, LED_AMOUNT);
}

void updateLedTimer(int timer)  {
  timerLedSequence->setInterval(timer);
  timerLedSequence->Start();
}

void updateLedTurnOffTimer(int timer)  {
  timerLedTurnOffSequence->setInterval((int)(timer * LED_TURN_OFF_SEQUENCE_FACTOR));
  timerLedTurnOffSequence->Start();
}

int decreaseInterval(int currentInterval) {
  if (currentInterval > 1000)
    return (int)(currentInterval * LED_SEQUENCE_DECREASE_INTERVAL_FACTOR);
  else if (currentInterval > 500)
    return (int)(currentInterval * (LED_SEQUENCE_DECREASE_INTERVAL_FACTOR + 0.030));
  else if (currentInterval > 300)
    return (int)(currentInterval * (LED_SEQUENCE_DECREASE_INTERVAL_FACTOR + 0.035));
  else if (currentInterval > 200)
    return (int)(currentInterval * (LED_SEQUENCE_DECREASE_INTERVAL_FACTOR + 0.040));
  else if (currentInterval > 100)
    return (int)(currentInterval * (LED_SEQUENCE_DECREASE_INTERVAL_FACTOR + 0.047));
  else if (currentInterval > 90)
    return currentInterval;
}

// called by timer
void ledTurnOffSequence() {
  turnOffAllLeds();
}

void checkForInputs()	{
  checkButtonInput(0, buttonGreenState);
  checkButtonInput(1, buttonBlueState);
  checkButtonInput(2, buttonWhiteState);
  checkButtonInput(3, buttonRedState);
}

/**
 * Checks for button input and changes the buttonState accordingly
 */
void checkButtonInput(int buttonIndex, int &buttonState) {
  const int buttonPin = buttons[buttonIndex];
  if (digitalRead(buttonPin) == HIGH && buttonState == 0) {
    if (millis() - timeOfLastButtonDebounce > DELAY_OF_BUTTON_DEBOUNCE) {
      buttonState = 1;
      timeOfLastButtonDebounce = millis();
      buttonPressed(buttonIndex);
    }
  } else if(digitalRead(buttonPin) == LOW && buttonState == 1)  {
    buttonState = 0;
  }
}

bool addToPlayStack(int number)	{
  if (playStackIndex >= PLAY_STACK_LENGTH) {
    return false;
  }
  playStack[playStackIndex] = number;
  playStackIndex++;
  debug_printPlayStack();
  return true;
}

void buttonPressed(int buttonIndex) {
  if (DEBUG == 1 || DEBUG == 2) {
    Serial.println("BUTTON PRESS: " + (String)buttonIndex);
  }
  if (isGameOver) {
    resetGame();
    return;
  }
  if(checkPlayStack(buttonIndex)) {
    playerScore++;
    updateDisplayScore();
    if (isRecentLedPressed())
      turnOffLed(buttonIndex);
  } else {
    gameOver();
  }
}

bool isRecentLedPressed() {
  return playStackIndex == 0;
}

bool checkPlayStack(int buttonIndex)	{
  if (playStack[0] == buttonIndex)  {
    removeFirstElementFromPlayStack();
    debug_printPlayStack();
    playStackIndex--;
    return true;
  } else {
    return false;
  }
}

void removeFirstElementFromPlayStack()  {
  for(int i = 0; i < PLAY_STACK_LENGTH; i++)  {
    if (i == PLAY_STACK_LENGTH - 1) {
      playStack[i] = -1;
    } else {
      playStack[i] = playStack[i + 1];
    }
  }
}

void lidLed(int ledIndex)	{
  if (DEBUG == 1)
    Serial.println("LED: " + (String)ledIndex + " - ON");
  if (ledIndex < LED_AMOUNT)  {
    digitalWrite(leds[ledIndex], HIGH);
  }
}

void lidAllLeds() {
  for(int i = 0; i < LED_AMOUNT; i++) {
    lidLed(i);
  }
}

void turnOffLed(int ledIndex) {
  if (DEBUG == 1)
    Serial.println("LED: " + (String)ledIndex + " – OFF");
  if (ledIndex < LED_AMOUNT) {
    digitalWrite(leds[ledIndex], LOW);
  }
}

void turnOffAllLeds() {
  for(int i = 0; i < LED_AMOUNT; i++) {
    turnOffLed(i);
  }
}

void updateDisplayScore() {
  displayOnScreen(playerScore);
}

void displayOnScreen(int toBeDisplayed)  {
  // TODO: implementation for the 7 segment display here

  Serial.println("DISPLAY: " + (String)toBeDisplayed);
}

void gameOver()	{
  Serial.println("---------");
  Serial.println("GAME OVER");
  Serial.println("---------");

  timerLedSequence->Stop();
  timerLedTurnOffSequence->Stop();
  updateDisplayScore();
  isGameOver = true;

  blinkLeds(500);
  blinkLeds(500);
  blinkLeds(500);
  blinkLeds(500);
}

void resetGame()  {
  Serial.println("++++++++++++++");
  Serial.println("GAME RESET!");
  Serial.println("++++++++++++++");
  
  // initGame() - all shared initializations.
  
  playerScore = 0;
  ledSequenceInterval = DEFAULT_PLAY_SEQUENCE;
  resetPlayStack();
  updateDisplayScore();
  timerLedSequence->Start();
  timerLedTurnOffSequence->Start();
  isGameOver = false;
  randomSeed(analogRead(0));
  startupLedBlinking();
}

void resetPlayStack() {
  for (int i = 0; i < PLAY_STACK_LENGTH; i++) {
    playStack[i] = -1;
  }
  playStackIndex = 0;
}

void startupLedBlinking()  {
  scrollLeds(200);
  blinkLeds(500);
  blinkLeds(500);
}

// delay in use
void blinkLeds(int delayAmount) {
  lidAllLeds();
  delay(delayAmount);
  turnOffAllLeds();
  delay(delayAmount);
}

// delay in use
void scrollLeds(int delayAmount) {
  lidLed(0);
  delay(delayAmount);
  turnOffAllLeds();
  lidLed(1);
  delay(delayAmount);
  turnOffAllLeds();
  lidLed(2);
  delay(delayAmount);
  turnOffAllLeds();
  lidLed(3);
  delay(delayAmount);
  turnOffAllLeds();
  lidLed(2);
  delay(delayAmount);
  turnOffAllLeds();
  lidLed(1);
  delay(delayAmount);
  turnOffAllLeds();
}

/* DEBUG */
void debug_printPlayStack()  {
  if (DEBUG != 1)
    return;
  for(int i = 0; i < PLAY_STACK_LENGTH; i ++)  {
    Serial.println("playStack: i = " + (String)i  + ": " + playStack[i]);
  }
  Serial.println("playStackIndex:" + (String)playStackIndex);
}
