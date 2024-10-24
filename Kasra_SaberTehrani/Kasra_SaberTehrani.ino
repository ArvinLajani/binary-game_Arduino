#include <LiquidCrystal_I2C.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <EnableInterrupt.h>



// Define pin numbers for each LED and button
#define L1 8   // MSB
#define L2 9
#define L3 10
#define L4 11  // LSB
#define LS 12  // Red LED for error and pulsing

#define BUTTON_PIN1 2
#define BUTTON_PIN2 3
#define BUTTON_PIN3 4
#define BUTTON_PIN4 5

#define POT_PIN A0   // Potentiometer pin for difficulty

// Store the last button states and LED states for all buttons and LEDs
byte lastButtonState1 = LOW;
byte lastButtonState2 = LOW;
byte lastButtonState3 = LOW;
byte lastButtonState4 = LOW;

byte ledState1 = LOW;
byte ledState2 = LOW;
byte ledState3 = LOW;
byte ledState4 = LOW;

// Debounce handling
unsigned long debounceDuration = 50; // 50 milliseconds debounce time
unsigned long lastTimeButtonStateChanged1 = 0;
unsigned long lastTimeButtonStateChanged2 = 0;
unsigned long lastTimeButtonStateChanged3 = 0;
unsigned long lastTimeButtonStateChanged4 = 0;

// Sleep mode variable
unsigned long lastButtonPressTime = 0; // To track last button press

int decimalValue = 0;  // To store the decimal value of the binary LEDs
int randomNum = 0;     // Store the random number
int score = 0;         // Player's score

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

// Variables for non-blocking timing
unsigned long previousMillis = 0;   // Store last time event occurred
const long baseInterval = 100000;    // Base time interval for a round (10 seconds)
unsigned long roundStartTime;       // Time when the round starts
bool userInputPhase = true;         // Flag to know if waiting for user input

// Red LED pulsing variables
int brightness = 0;          // Brightness level for the red LED
int fadeAmount = 5;          // How much to change the brightness each loop
unsigned long lastFadeUpdate = 0; // Store the last time brightness was updated
const long fadeInterval = 30; // Speed of the fade

// Variables for difficulty adjustment
int difficulty = 1;  // Difficulty level (1-4)
float factorF = 1.0; // Time reduction factor based on difficulty
String difficultyLabel = "";  // Label for difficulty level

//time after apply pot
 unsigned long T1 = baseInterval / factorF;

//kap idea for sleepinmode power by AI

 const unsigned long inactivityTimeout = 10000;  // 10 seconds
 bool sleepatleastOncw=true;
void setup() {
  // Set each LED pin to OUTPUT and button pin to INPUT
  Serial.begin(9600); 
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(L4, OUTPUT);
  pinMode(LS, OUTPUT);

  pinMode(BUTTON_PIN1, INPUT);
  pinMode(BUTTON_PIN2, INPUT);
  pinMode(BUTTON_PIN3, INPUT);
  pinMode(BUTTON_PIN4, INPUT);
  
  // Potentiometer pin for difficulty
  pinMode(POT_PIN, INPUT);

  lcd.init();
  lcd.backlight();
//  while (digitalRead(BUTTON_PIN1) == HIGH) {
  
//   lcd.setCursor(0, 0);
//   lcd.print("Welcome to GMB!");
//   lcd.setCursor(0, 1);
//   lcd.print("Set Difficulty & Press B1");

//       setDifficulty();

//   // Display difficulty level for the player
  
//   lcd.setCursor(0, 2);
//   lcd.print("Difficulty: ");
//   lcd.print(difficultyLabel);  // Show label like "Easy", "Medium", etc.
  

//   }
  resetGame();
}

void loop() {

  
  //loop for chwcking secondtime sleeping
//     if(sleepatleastOncw){
//       ///kosshero
//       score = 0;
//   ///add
//   //digitalWrite(BUTTON_PIN1, LOW);
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Welcome to GMB!");
//   lcd.setCursor(0, 1);
//   lcd.print("Set Difficulty then");
//   lcd.setCursor(0, 2);
//   lcd.print("Press B1");
//   ledandbuttonbeoff();  // Ensure LEDs are off

//   // Wait for the player to press B1 to start the game
//   while (digitalRead(BUTTON_PIN1) == HIGH) {
//     pulseRedLed();  // Pulse the red LED while waiting
     

//       setDifficulty();
//       checkInactivity();
//   // Display difficulty level for the player
  
//   lcd.setCursor(0,3);
//   lcd.print("Difficulty:");
//  // int temp = baseInterval / difficulty;
//   lcd.print(difficultyLabel);  // Show label like "Easy", "Medium", etc.
//    //lcd.print(temp/1000);
//   }
//    T1 = T1/factorF;

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Go!");
//   delay(1000);  // Small delay before starting the game
//   generateRandomNumber();  // Start the first round
//   userInputPhase = true;
//   roundStartTime = millis();
//   sleepatleastOncw = false;
  //  }

  unsigned long currentMillis = millis();
   
  // Adjust the time limit for the round based on difficulty
   
  int showtime=(currentMillis - roundStartTime)/1000 ;
   int countime = (T1/1000) - showtime;
  if (userInputPhase) {
    // Handle button presses and LED updates with debounce
    handleButtonPresses();

    // Update the decimal value from the LED states
    updateDecimalValue();

    // Display the player's current input on the LCD
    lcd.setCursor(0, 2);
    lcd.print("Input: ");
    
     lcd.setCursor(8, 2);
    lcd.print(decimalValue);
    lcd.print("    ");  // Clear leftover digits
    lcd.setCursor(0, 3);
    lcd.print(countime);
   // delay(1000);
   // lcd.setCursor(0, 3);   // Set cursor to the first line
    lcd.print("    ");  // Print 16 spaces to "clear" the first line

    // Check if the user has run out of time
    if (currentMillis - roundStartTime > T1) {
      gameOver();  // Time is up, game over
    }

    // Check if the user's input matches the random number
    if (decimalValue == randomNum) {
      lcd.setCursor(0, 3);
      lcd.print("Correct!");
      score++;  // Increment score
      userInputPhase = false;  // Move to next phase
      previousMillis = currentMillis;  // Reset the timer
      T1 = T1/factorF;
    }
  } else {
    // Show "Correct" for 2 seconds before moving to the next round
    if (currentMillis - previousMillis >= 2000) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Next Round:");
      generateRandomNumber();  // Generate a new random number
      userInputPhase = true;
      roundStartTime = millis();  // Start the timer for the new round
      ledandbuttonbeoff();  // Reset LEDs and buttons
      //sleepatleastOncw = false;
    }
  }
 // checkInactivity();
}

void resetGame() {
  score = 0;
  ///add
  //digitalWrite(BUTTON_PIN1, LOW);
  //currentMillis=0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to GMB!");
  lcd.setCursor(0, 1);
  lcd.print("Set Difficulty then");
  lcd.setCursor(0, 2);
  lcd.print("Press B1");
  ledandbuttonbeoff();  // Ensure LEDs are off

  // Wait for the player to press B1 to start the game
  while (digitalRead(BUTTON_PIN1) == HIGH) {
    pulseRedLed();  // Pulse the red LED while waiting
     

      setDifficulty();
      //if(sleepatleastOncw)
      checkInactivity();
  // Display difficulty level for the player
  
  lcd.setCursor(0,3);
  lcd.print("Difficulty:");
 // int temp = baseInterval / difficulty;
  lcd.print(difficultyLabel);  // Show label like "Easy", "Medium", etc.
   //lcd.print(temp/1000);
  }
   T1 = T1/factorF;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Go!");
  delay(1000);  // Small delay before starting the game
  generateRandomNumber();  // Start the first round
  userInputPhase = true;
  roundStartTime = millis();  // Start the timer for the first round
  T1 = baseInterval / factorF;
}

void gameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Final Score: ");
  lcd.print(score);

  // Show the red LED for 1 second to indicate the end
  digitalWrite(LS, HIGH);
  delay(1000);
  digitalWrite(LS, LOW);

  delay(5000);  // Hold the game over screen for 5 seconds
  lastButtonPressTime = millis();
  resetGame();  // Restart the game
}

void setDifficulty() {
  int potValue = analogRead(POT_PIN);  // Read the potentiometer value (0 to 1023)

  // Map the potentiometer value to difficulty level
  if (potValue < 256) {
    difficulty = 1;
    factorF = 1.09;  // No time reduction
    difficultyLabel = "Easy Mode";
  } else if (potValue < 612) {
    difficulty = 2;
    factorF = 1.21;  // Moderate time reduction
    difficultyLabel = "Medium Mode";
  } else if (potValue < 868) {
    difficulty = 3;
    factorF = 1.69;  // Significant time reduction
    difficultyLabel = "Hard Mode";
  } else {
    difficulty = 4;
    factorF = 1.85;  // Extreme time reduction
    difficultyLabel = "God Mode";
  }
}

// Function to generate a random number and display it on the LCD
void generateRandomNumber() {
  ledState1 = LOW;
  ledState2 = LOW;
  ledState3 = LOW;
  ledState4 = LOW;

  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  digitalWrite(L3, LOW);
  digitalWrite(L4, LOW);

  decimalValue = 0;
  randomNum = random(0, 16);
  lcd.setCursor(0, 1);
  lcd.print("Number: ");
  lcd.print(randomNum);
  lcd.print("    ");  // Clear leftover digits
  roundStartTime = millis();  // Start the timer for the new round
}

// Function to handle button presses and debounce logic
void handleButtonPresses() {
  if (millis() - lastTimeButtonStateChanged1 > debounceDuration) {
    byte buttonState1 = digitalRead(BUTTON_PIN1);
    if (buttonState1 != lastButtonState1) {
      lastTimeButtonStateChanged1 = millis();
      lastButtonPressTime = millis();
      lastButtonState1 = buttonState1;
      if (buttonState1 == LOW) {
        ledState1 = (ledState1 == HIGH) ? LOW : HIGH;
        digitalWrite(L1, ledState1);
      }
    }
  }

  if (millis() - lastTimeButtonStateChanged2 > debounceDuration) {
    byte buttonState2 = digitalRead(BUTTON_PIN2);
    if (buttonState2 != lastButtonState2) {
      lastTimeButtonStateChanged2 = millis();
      lastButtonPressTime = millis();
      lastButtonState2 = buttonState2;
      if (buttonState2 == LOW) {
        ledState2 = (ledState2 == HIGH) ? LOW : HIGH;
        digitalWrite(L2, ledState2);
      }
    }
  }

  if (millis() - lastTimeButtonStateChanged3 > debounceDuration) {
    byte buttonState3 = digitalRead(BUTTON_PIN3);
    if (buttonState3 != lastButtonState3) {
      lastTimeButtonStateChanged3 = millis();
      lastButtonPressTime = millis();
      lastButtonState3 = buttonState3;
      if (buttonState3 == LOW) {
        ledState3 = (ledState3 == HIGH) ? LOW : HIGH;
        digitalWrite(L3, ledState3);
      }
    }
  }

  if (millis() - lastTimeButtonStateChanged4 > debounceDuration) {
    byte buttonState4 = digitalRead(BUTTON_PIN4);
    if (buttonState4 != lastButtonState4) {
      lastTimeButtonStateChanged4 = millis();
      lastButtonPressTime = millis();
      lastButtonState4 = buttonState4;
      if (buttonState4 == LOW) {
        ledState4 = (ledState4 == HIGH) ? LOW : HIGH;
        digitalWrite(L4, ledState4);
      }
    }
  }
}

// Function to update the decimal value from the binary LED states
void updateDecimalValue() {
  decimalValue = (ledState1 << 3) | (ledState2 << 2) | (ledState3 << 1) | ledState4;
}

// Function to pulse the red LED during the initial waiting phase
void pulseRedLed() {
  if (millis() - lastFadeUpdate >= fadeInterval) {
    brightness += fadeAmount;
    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;
    }
    analogWrite(LS, brightness);
    lastFadeUpdate = millis();
  }
}
void enterSleep() {
  // Disable ADC to save power
  ADCSRA &= ~(1 << ADEN);
  
  // Set sleep mode to power down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Attach interrupts to wake up from any button press
   //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN1), wakeUp, LOW);
  // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN2), wakeUp, LOW);
   //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN3), wakeUp, LOW);
  // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN4), wakeUp, LOW);

  enableInterrupt(digitalPinToInterrupt(BUTTON_PIN1), wakeUp, RISING);
  enableInterrupt(digitalPinToInterrupt(BUTTON_PIN2), wakeUp, RISING);
  enableInterrupt(digitalPinToInterrupt(BUTTON_PIN3), wakeUp, RISING);
  enableInterrupt(digitalPinToInterrupt(BUTTON_PIN4), wakeUp, RISING);
  // Go to sleep
  sleep_cpu();

  // Execution resumes here after waking up
  sleep_disable();  // Disable sleep after waking up
 // detachInterrupt(digitalPinToInterrupt(BUTTON_PIN1));
  // detachInterrupt(digitalPinToInterrupt(BUTTON_PIN2));
   //detachInterrupt(digitalPinToInterrupt(BUTTON_PIN3));
   //detachInterrupt(digitalPinToInterrupt(BUTTON_PIN4));

  // Re-enable ADC
  ADCSRA |= (1 << ADEN);

  // Wake up message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waking up...");
  delay(1000);  // Small delay to show wake-up message
 //digitalWrite(BUTTON_PIN1,LOW);
 sleepatleastOncw = false;
 score = 0;
  ///add
  //digitalWrite(BUTTON_PIN1, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to GMB!");
  lcd.setCursor(0, 1);
  lcd.print("Set Difficulty then");
  lcd.setCursor(0, 2);
  lcd.print("Press B1");
  ledandbuttonbeoff();  // Ensure LEDs are off
   //loop ();  // Restart game
}
void wakeUp() {
  // This function will be called when an interrupt is triggered to wake the system up
  // No need to do anything here, the system will automatically wake up
}


// Function to check for inactivity and go to sleep
void checkInactivity() {
  //handleButtonPresses();
  unsigned long currentMillis = millis();
  if ((currentMillis - lastButtonPressTime > inactivityTimeout)) {
     Serial.print("lastTimeButtonStateChanged1: ");
     Serial.println(lastTimeButtonStateChanged1);
     lastButtonPressTime = lastButtonPressTime +10000;
    // Serial. println("lastButtonPressTime" , lastButtonPressTime );
     Serial.print("lastButtonPressTime:" );
     Serial.println(lastButtonPressTime);

     Serial.print("currentMillis:" );
     Serial.println(currentMillis);

     Serial.print("roundStartTime:" );
     Serial.println(lastButtonPressTime);

     Serial.print("lastButtonPressTime:" );
     Serial.println(lastButtonPressTime);
     Serial. println(millis() );
    //  Serial. println(lastButtonPressTime );
    //  Serial. println(lastButtonPressTime );
    //  Serial. println(lastButtonPressTime );
    // No button press for 10 seconds, enter sleep mode
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sleeping...");
    delay(1000);  // Small delay to show the sleep message
    enterSleep();
  }
}
// Function to turn off all LEDs and buttons
void ledandbuttonbeoff() {
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  digitalWrite(L3, LOW);
  digitalWrite(L4, LOW);
  digitalWrite(LS, LOW);
}