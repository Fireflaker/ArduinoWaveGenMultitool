#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <toneAC.h>

#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {12, 8, 7, 6}; //connect to the column pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

long Freq = 1000;
int FreqGendone = 0;
char holdKey;
unsigned long t_hold;


void setup() {
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  //===display.display();
  //===delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  splash();
  // Invert and restore display, pausing in-between
  display.invertDisplay(true);
  delay(500);
  display.invertDisplay(false);
  delay(500);

  Serial.println(F("Setup SPLASH Finished"));

  //  for (unsigned long freq = 125; freq <= 15000; freq += 100) {
  //    toneAC(freq); // Play the frequency (125 Hz to 4MHz sweep in 1 Hz steps).
  //    // delay(1);     // Wait 1 ms so you can hear it.
  //  }
  //  delay(1000); // Wait a second.
  //
  //  Serial.println(F("Setup SWEEP Finished"));
  //  toneAC(); // Turn off toneAC, can also use noToneAC().
  Serial.println(F("Setup Finished"));
  selectMode();
}

void loop() {

  char key = keypad.getKey();
  if (key) {
    Serial.print("In loop =");
    Serial.println(key);
  }
  selectMode();

}

void splash(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  display.println(F("WaveGen2"));

  display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  display.println("FLKR");

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.print(F("up=")); display.println(millis());

  display.display();

}


void selectMode(void) {

  keypad.addEventListener(keypadEvent); // Add an event listener for this keypad

  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  display.println(F("selectMode"));

  display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  display.println("A. Hard 50% Freq Gen");

  display.setTextSize(1);             // Draw 2nd line text
  display.setTextColor(WHITE);
  display.println(F("B. PWM"));

  display.setTextSize(1);             // Draw 2nd line text
  display.setTextColor(WHITE);
  display.print(F("C. SWEEP"));

  display.display();
  delay(100);
  Serial.println(F("SelectMode"));
}

void keypadEvent(KeypadEvent key) {
  if (key) {
    holdKey = key;
  }
  switch (keypad.getState()) {
    case PRESSED:

      if (key == 'A') {
        Serial.println(F("*******Mode A_Hard_50%_Freq_Gen Selected*******"));
        FreqGen();
      }
      break;

      //    case HOLD:
      //      Serial.println("*Is Held");
      //
      //      if ((millis() - t_hold) > 100 ) {
      //        switch (holdKey) {
      //          case '2':
      //            Serial.println("*Freq Up");
      //            display.println(Freq);
      //            displayFreqInHardGen();
      //            break;
      //          case '8':
      //            Serial.println("*Freq Down");
      //            display.println(Freq);
      //            displayFreqInHardGen();
      //            break;
      //        }
      //        t_hold = millis();
      //      }
      //        if (key == '2') {
      //          Freq++;
      //          Serial.println("*Freq Up");
      //          display.println(Freq);
      //        }
      //        break;
      //
      //        if (key == '8') {
      //          Freq--;
      //          Serial.println("*Freq Down");
      //          display.println(Freq);
      //        }
      //        break;
      //
      //    case RELEASED:
      //
      //      break;
      //
      //    case HOLD:
      //        if (key == 'A') {
      //            blink = true;    // Blink the LED when holding the * key.
      //        }
      //        break;
  }
}




void FreqGen() {
  Serial.println(F("FreqGen"));

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  display.println(F("Hard_50%_Freq_Gen"));
  display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  display.println(" Frequency?Hz  end=# ");
  display.setTextSize(2);
  display.setCursor(90, 16);            // Start at place for number
  display.println(" Hz");
  display.display();

  Freq = GetNumber();

  Serial.println("FreqSet=");
  Serial.print(Freq);
  char key = keypad.getKey();
  toneAC(Freq); // before while so it can be turned off
  while (key != '*') {
    int toggle = 0;
    switch (key) {
      case NO_KEY:
        break;
      case '2':
        Freq++;
        break;
      case '1':
        toggle = 1;
        //        Serial.println("before getstate");
        //        if(keypad.getState() == HOLD){Serial.println("keypad.getState() == HOLD is valid");} // it is not!!!!! WORK HERE!!!!!!
        //        Serial.println(String(keypad.getState()));
        while (key != '1' || toggle == 1) {
          Serial.println("after getstate");
          Freq++;
          displayFreqInHardGen();
          toneAC(Freq);
          key = keypad.getKey();
          toggle = 0;
        }
        break;
      case '8':
        Freq--;
        break;
      case '7':
        toggle = 1;
        while (key != '7' || toggle == 1) {
          Freq--;
          displayFreqInHardGen();
          toneAC(Freq);
          key = keypad.getKey();
          toggle = 0;
        }
        break;
      case '3':
        toggle = 1;
        while (key != '3' || toggle == 1) {
          Freq = Freq * 1.01;
          displayFreqInHardGen();
          toneAC(Freq);
          key = keypad.getKey();
          toggle = 0;
        }
        break;
      case '9':
        toggle = 1;
        while (key != '9' || toggle == 1) {
          Freq = Freq * 0.99;
          displayFreqInHardGen();
          toneAC(Freq);
          key = keypad.getKey();
          toggle = 0;
        }
        break;
      case '4':
        Freq = 2;
        displayFreqInHardGen();
        toneAC(Freq);
        toggle = 0;
        break;
      case '5':
        Freq = 1000;
        displayFreqInHardGen();
        toneAC(Freq);
        toggle = 0;
        break;
      case '6':
        Freq = 2000000;
        displayFreqInHardGen();
        toneAC(Freq);
        toggle = 0;
        break;
      case '#':
        toneAC();
        break;
    }
    key = keypad.getKey();
    displayFreqInHardGen();
  }

}



long GetNumber()
{
  long num = 0;
  char key = keypad.getKey();
  display.setCursor(0, 16);
  while (key != '#')
  {
    switch (key)
    {
      case NO_KEY:
        break;

      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':

        display.print(key);
        display.display();
        num = num * 10 + (key - '0');
        Serial.print("NumCurrent=");
        Serial.println(num);
        break;

      case '*':
        num = 0;
        FreqGen();
        break;
    }

    key = keypad.getKey();
  }
  // it is # pressed!!!!!
  //    if (num <= 0 || num > 5000000) {
  //      Serial.println("FreqSet Overflow!!!!!");
  //      num = 4000000;
  //    }
  return num;
}

void displayFreqInHardGen() {
  display.clearDisplay();                         // =screen after the frequency has been set, just the title "On"
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  display.setCursor(0, 0);
  display.println(F("*Hard_50%_Freq_Gen_On"));
  display.setTextSize(2);
  display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  display.print(Freq);
  display.setTextColor(WHITE);
  display.println(" Hz");
  display.display();
}
