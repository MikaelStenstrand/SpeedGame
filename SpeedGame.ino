#define PLAY_STACK_LENGTH 3

#define DEBUG 1

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

int randomIndex;

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
}


void loop()
{
  checkForInputs();
  
}




void checkForInputs()	{
  // TODO: Trigger new thread with the button press logic

  if (digitalRead(BUTTON_GREEN) == HIGH && buttonGreenState == 0) {
    if (millis() - timeOfLastDebounce > delayOfDebounce) {
      buttonGreenState = 1;
      timeOfLastDebounce = millis();
      playSequence();
    }
  } else if(digitalRead(BUTTON_GREEN) == LOW && buttonGreenState == 1)  {
    buttonGreenState = 0;
    turnOffAllLeds();  // TODO: not needed after timer is implemented
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

// lid random led and add it to the play sequence array
void playSequence()	{
  randomIndex = getRandomNumber();
  if (addToPlayStack(randomIndex))	{
    lidLed(randomIndex);
    
  } else	{
  	gameOver();
  }
}

int getRandomNumber()	{
  return random(1, 4); // TODO: 0, 4 
  // TODO: less same numbers
}

bool addToPlayStack(int number)	{
  // TODO: check if array is full, i.e. playSequenceIndex == array.length() - 1
  if (playStackIndex >= PLAY_STACK_LENGTH) {
    return false;
  }
  playStack[playStackIndex] = number;
  playStackIndex ++;
  debug_printPlayStack();
  return true;
}

void buttonPressed(int buttonIndex) {
  if(checkPlayStack(buttonIndex)) {
    // todo:
  } else {
    gameOver();
  }
}

bool checkPlayStack(int buttonIndex)	{
  if (playStack[0] == buttonIndex)  {
    Serial.println("MATCH!");
    removeFirstElementFromPlayStack();
    playerScore ++;
    playStackIndex --;
    return true;
  } else {
    return false;
  }  
}

void removeFirstElementFromPlayStack()  {
  for(int i = 0; i < PLAY_STACK_LENGTH; i++)  {
    if (i == PLAY_STACK_LENGTH - 1) {
      playStack[i] = 0;
    } else {
      playStack[i] = playStack[i + 1];
    }
  }
  debug_printPlayStack();
}

void lidLed(int ledIndex)	{
  Serial.println("lidLed: " + (String)ledIndex);
  if (ledIndex < 4)  {
    digitalWrite(leds[ledIndex], HIGH);
  }
}

void turnOffAllLeds() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_WHITE, LOW);
  digitalWrite(LED_RED, LOW);
}


void debug_printPlayStack()  {
  if (DEBUG != 1)
    return;
  for(int i = 0; i < PLAY_STACK_LENGTH; i ++)  {
    Serial.println("playStack: i = " + (String)i  + ": " + playStack[i]);
  }
  Serial.println("playStackIndex:" + (String)playStackIndex);
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
  Serial.println("---------");
  Serial.println("GAME OVER");
  Serial.println("---------");
  Serial.println("---------");
}






