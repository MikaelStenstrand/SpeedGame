#define PLAY_STACK_LENGTH 3

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
#define BUTTON_GREEN  10
#define BUTTON_BLUE	  11
#define BUTTON_WHITE  12
#define BUTTON_RED	  13

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
  if (digitalRead(BUTTON_GREEN) == HIGH && buttonGreenState == 0) {
    if (millis() - timeOfLastDebounce > delayOfDebounce) {
      buttonGreenState = 1;
      timeOfLastDebounce = millis();
      playSequence();
    }
  } else if(digitalRead(BUTTON_GREEN) == LOW && buttonGreenState == 1)  {
    buttonGreenState = 0;

    // TODO: debugg purposes
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_WHITE, LOW);
    digitalWrite(LED_RED, LOW);
  }
  // TODO: Trigger new thread with the button press logic
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
  return random(0, 4);
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

bool checkPlayStack(int buttonPress)	{
  // check if the button pressed is the correct in order of playSequenceList
  // if ok
	// playSqeuenceIndex --;
  	// remove first item from array
  	// rearrange all items in array [i] = [i+1]
  return true;
}

void lidLed(int ledIndex)	{
  Serial.println("lidLed: " + (String)ledIndex);
  if (ledIndex < 4)  {
    digitalWrite(leds[ledIndex], HIGH);
  }
}




void debug_printPlayStack()  {
  for(int i = 0; i < PLAY_STACK_LENGTH; i ++)  {
    Serial.println("playStack: i = " + (String)i  + ": " + playStack[i]);
  }
  Serial.println("playStackIndex:" + (String)playStackIndex);
}



void gameOver()	{
  Serial.println("---------");
  Serial.println("---------");
  Serial.println("GAME OVER");
  Serial.println("---------");
  Serial.println("---------");
}






