#include "TimerObject.h"

#define PLAY_STACK_LENGTH 3
#define LED_AMOUNT 4
#define DEFAULT_PLAY_SEQUENCE 2000

#define DEBUG 2

#define LED_GREEN 2
#define LED_BLUE  3
#define LED_WHITE 4
#define LED_RED   5

const int leds[] = {
  LED_GREEN,
  LED_BLUE,
  LED_WHITE,
  LED_RED,
};

// BUTTONS
#define BUTTON_GREEN  9
#define BUTTON_BLUE	  10
#define BUTTON_WHITE  11
#define BUTTON_RED	  12

int buttonGreenState 	= 0;
int buttonBlueState		= 0;
int buttonWhiteState	= 0;
int buttonRedState		= 0;

long timeOfLastDebounce = 0;
long delayOfDebounce = 100;

// PLAY LOGIC
int playStack[PLAY_STACK_LENGTH];
int playStackIndex = 0;
int playerScore = 0;

int ledSequenceInterval = DEFAULT_PLAY_SEQUENCE;
float ledSequenceDecreaseIntervalFactor = 0.95;
float ledTurnOffSequenceFactor = 0.60;
int randomIndex;
bool isGameOver = false;

TimerObject *timerLedSequence = new TimerObject(ledSequenceInterval);
TimerObject *timerLedTurnOffSequence = new TimerObject((int)(ledSequenceInterval * ledTurnOffSequenceFactor));

void setup()
{
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


void loop()
{
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
  randomIndex = getRandomNumber(0, 4);
  if (addToPlayStack(randomIndex))	{
    lidLed(randomIndex);
    ledSequenceInterval = decreaseInterval(ledSequenceInterval);
    updateLedTimer(ledSequenceInterval);
    updateLedTurnOffTimer(ledSequenceInterval);
  } else	{
  	gameOver();
  }
}

void updateLedTimer(int timer)  {
  timerLedSequence->setInterval(timer);
  timerLedSequence->Start();
}

void updateLedTurnOffTimer(int timer)  {
  timerLedTurnOffSequence->setInterval((int)(timer * ledTurnOffSequenceFactor));
  timerLedTurnOffSequence->Start();
}

int decreaseInterval(int currentInterval) {
  if (currentInterval > 1000)
    return (int)(currentInterval * ledSequenceDecreaseIntervalFactor);
  else if (currentInterval > 500)
    return (int)(currentInterval * (ledSequenceDecreaseIntervalFactor + 0.030));
  else if (currentInterval > 300)
    return (int)(currentInterval * (ledSequenceDecreaseIntervalFactor + 0.035));
  else if (currentInterval > 200)
    return (int)(currentInterval * (ledSequenceDecreaseIntervalFactor + 0.040));
  else if (currentInterval > 100)
    return (int)(currentInterval * (ledSequenceDecreaseIntervalFactor + 0.047));
  else if (currentInterval > 90)
    return currentInterval;
}

// called by timer
void ledTurnOffSequence() {
  turnOffAllLeds();
}

void checkForInputs()	{
  if (digitalRead(BUTTON_GREEN) == HIGH && buttonGreenState == 0) {
    if (millis() - timeOfLastDebounce > delayOfDebounce) {
      buttonGreenState = 1;
      timeOfLastDebounce = millis();
      buttonPressed(0);
    }
  } else if(digitalRead(BUTTON_GREEN) == LOW && buttonGreenState == 1)  {
    buttonGreenState = 0;
  }

  if (digitalRead(BUTTON_BLUE) == HIGH && buttonBlueState == 0) {
    if (millis() - timeOfLastDebounce > delayOfDebounce) {
      buttonBlueState = 1;
      timeOfLastDebounce = millis();
      buttonPressed(1);
    }
  } else if(digitalRead(BUTTON_BLUE) == LOW && buttonBlueState == 1)  {
    buttonBlueState = 0;
  }

  if (digitalRead(BUTTON_WHITE) == HIGH && buttonWhiteState == 0) {
    if (millis() - timeOfLastDebounce > delayOfDebounce) {
      buttonWhiteState = 1;
      timeOfLastDebounce = millis();
      buttonPressed(2);
    }
  } else if(digitalRead(BUTTON_WHITE) == LOW && buttonWhiteState == 1)  {
    buttonWhiteState = 0;
  }

  if (digitalRead(BUTTON_RED) == HIGH && buttonRedState == 0) {
    if (millis() - timeOfLastDebounce > delayOfDebounce) {
      buttonRedState = 1;
      timeOfLastDebounce = millis();
      buttonPressed(3);
    }
  } else if(digitalRead(BUTTON_RED) == LOW && buttonRedState == 1)  {
    buttonRedState = 0;
  }
}

int getRandomNumber(int startNumber, int endNumber)	{
  return random(startNumber, endNumber);
}

bool addToPlayStack(int number)	{
  if (playStackIndex >= PLAY_STACK_LENGTH) {
    return false;
  }
  playStack[playStackIndex] = number;
  playStackIndex ++;
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
    playerScore ++;
    updateDisplayScore();
    if (playStackIndex == 0) 
      turnOffLed(buttonIndex);
  } else {
    gameOver();
  }
}

bool checkPlayStack(int buttonIndex)	{
  if (playStack[0] == buttonIndex)  {
    removeFirstElementFromPlayStack();
    debug_printPlayStack();
    playStackIndex --;
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
  Serial.println("GAME RESETTED!");
  Serial.println("++++++++++++++");

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
