#include <SPI.h>
#include <Wire.h>
#include <Keyboard.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "macro.h"

#define macroArrayLen (sizeof(macroArray) / sizeof(String))

#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    6
#define OLED_CS    7
#define OLED_RESET 8
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

int selectedMacro = 0;

int button1State;
int lastButton1State = LOW;
int button2State;
int lastButton2State = LOW;
int button3State;
int lastButton3State = LOW;
int button4State;
int lastButton4State = LOW;

int changeModeLen = 0;
int changeModeDelay = 0;

unsigned long button1debounceTime = 0;
unsigned long button2debounceTime = 0;
unsigned long button3debounceTime = 0;
unsigned long button4debounceTime = 0;

bool waitUntilKey2Up = false;
bool waitUntilKey4Up = false;

unsigned long debounceDelay = 200;

int lineShift = 0;
int lineLen = 50;

void setup()   {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  Serial.begin(9600);
  Keyboard.begin();

  display.begin(SSD1306_SWITCHCAPVCC);

  // Clear the buffer.
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("SMART MACRO BOARD");
  display.display();
  delay(500);

  display.setCursor(0,20);
  display.println("Initializing");
  display.display();
  delay(500);

  display.setCursor(0,30);
  display.print("Found ");
  display.print(macroArrayLen);
  display.print(" macroses");
  display.display();

  delay(2000);
}

void loop() {
    display.clearDisplay();

    display.drawLine(lineShift, 11, lineShift + lineLen, 11, WHITE);

    lineShift++;

    if((lineShift + lineLen) > 128 + lineLen){
      lineShift = 0 - lineLen;
    }

    int reading1 = digitalRead(A0);
    int reading2 = digitalRead(A1);
    int reading3 = digitalRead(A2);
    int reading4 = digitalRead(A3);

    if (reading1 != button1State) {
        button1State = reading1;
        button1debounceTime = 0;
    }

    if(millis() - button1debounceTime > debounceDelay){
      button1debounceTime = millis();
      if(button1State== HIGH){
        selectedMacro--;
      }
    }

    if (reading2 != button2State) {
      button2State = reading2;
    }

    if (reading3 != button3State) {
      button3State = reading3;
      button3debounceTime = 0;
    }

    if(millis() - button3debounceTime > debounceDelay){
      button3debounceTime = millis();
      if(button3State== HIGH){
        selectedMacro++;
      }
    }

    if (reading4 != button4State) {
      button4State = reading4;
    }

    if(selectedMacro == macroArrayLen){
      selectedMacro = 0;
    }

    if(selectedMacro < 0){
      selectedMacro = macroArrayLen - 1;
    }

    if(button2State== HIGH){
      if((millis() - changeModeDelay) > 5){
        changeModeLen = changeModeLen + 3;
        changeModeDelay = millis();
      }else{
        changeModeDelay++;
      }
    }else{
      waitUntilKey2Up = false;
      changeModeLen = 0;
      changeModeDelay = 0;
    }

    if(button4State== HIGH){
      if(!waitUntilKey4Up){
        Serial.println(macroArray[selectedMacro]);
        Keyboard.print(macroArray[selectedMacro]);
        Keyboard.write(13);
        Keyboard.write(10);

        delay(50);
        waitUntilKey4Up = false;
      }
    }else{
      waitUntilKey4Up = false;
    }
    
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("#");
    display.print(selectedMacro + 1);
    display.print("/");
    display.print(macroArrayLen);
    display.print(" HID SERIAL");
    
    display.setTextSize(1);
    display.setCursor(0, 15);
    display.println(macroArray[selectedMacro]);

    if(changeModeLen < 0){
      changeModeLen = 0;
    }

    if(changeModeLen > 0){
      if(!waitUntilKey2Up){
        display.clearDisplay();

        display.setTextSize(2);
        display.setCursor(5, 10);
//        display.println("Change \r\n mode?");
        
        display.drawLine(0, 63, changeModeLen, 63, WHITE);
      }
    }

    if(changeModeLen >= 128){
      waitUntilKey2Up = true;
    }

    display.display();

    delay(2);
}
