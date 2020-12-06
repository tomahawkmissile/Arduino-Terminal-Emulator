#include <SPI.h>          // f.k. for Arduino-1.5.2
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#include <Wire.h>
#include <TouchScreen.h>

#include <String.h>

void setup(void);
void loop(void);

// Touch constants

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

#define MIN_PRESSURE 10
#define MAX_PRESSURE 300

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void resetTFT() {
  Serial.println(F("Initializing Arduino terminal emulator"));
    
  tft.reset();

  delay(150);

  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(2);
  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.fillRect(0,0,tft.width(),tft.height(),0x0000);

}

void setup() {
  Serial.begin(9600);

  resetTFT();
}

String cleanseString(String input) {
  int count=0;
  for(int idx=0;idx<input.length();idx++) {
    switch(input[idx]) {
      case '\n':
      case '\r':
        break;
      default:
        count++;
        break;
    }
  }
  char output[count];
  int offs=0;
  for(int idx=0;idx<count;idx++) {
    switch(input[idx]) {
      case '\n':
      case '\r':
        offs++;
        break;
      default:
        output[idx]=input[idx+offs];
        break;
    }
  }
  return String(output);
}

int y_index=0;
const int line_height = 8;

int getReturnCount(String input) {
  int count=0;
  for(int idx=0;idx<input.length();idx++) {
    if(input[idx]=='\n' || input[idx]=='\r') {
      count++;
    }
  }
  return count;
}

int c=1;

void screenPrint(String text) {
  int16_t x1,y1;
  uint16_t w,h;
  tft.getTextBounds(text,0,0,&x1,&y1,&w,&h);
  y_index+=h;
  
  //TODO: implement scrolling
  bool alreadyScrolled = false;
  bool justCleared = false;
  
  if(y_index >= tft.height()) {
    y_index=0;
    tft.fillScreen(0x0000);
    tft.setCursor(0,0);
    justCleared = true;
    tft.getTextBounds(text,0,0,&x1,&y1,&w,&h);
  }
  Serial.println(y_index);
  tft.print(text);
}

//Configuration values
int max_str_length = 512;

//TODO: Argument parser for custom commands. Returns false if no args recognized.
bool argsParser(String input) {
  if (input.substring(0,6).equals("COLOR=")) {
    tft.setTextColor(input.substring(6).toInt());
  } else if(input.equals("RESET")) {
    resetTFT();
  } else {
    return false;
  }
  return true;
}

void processInput(String incoming) {
  if(!argsParser(incoming)) {
    screenPrint(incoming);
  }
}

String cmd = "";

void loop() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if(p.z > MIN_PRESSURE && p.z < MAX_PRESSURE) {
    Serial.println(String("Touch registered at: (")+p.x+String(",")+p.y+String(")"));
  }
  
  if(Serial.available() > 0) {
    cmd += (char)Serial.read();
    if(cmd.length()>255) {
      processInput(cmd);
      cmd = "";
    }
  } else if(!cmd.equals("")) {
    processInput(cmd);
    cmd = "";
  }
}
