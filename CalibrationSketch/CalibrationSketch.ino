//
//    FILE: HX_calibration.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: HX711 demo
//     URL: https://github.com/RobTillaart/HX711


#include "HX711.h"
#include <M5StickCPlus.h>
#include <vector>
#include <numeric>

using namespace std;

vector<double> offsets;
vector<double> scales;

HX711 myScale;

uint8_t dataPin = 33;
uint8_t clockPin = 32;

uint32_t start, stop;
volatile float f;


void setup()
{
  M5.begin();
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  myScale.begin(dataPin, clockPin);
}

void loop()
{
  M5.update();
  calibrate();
  findAverageOffsetAndScale();
}

void findAverageOffsetAndScale() {
  
  double offsetsSum = accumulate(offsets.begin(), offsets.end(), 0);

  double finalOffset = static_cast<double>(offsetsSum) / offsets.size();

  double scalesSum = accumulate(scales.begin(), scales.end(), 0);

  double finalScale = static_cast<double>(scalesSum) / offsets.size();

  M5.Lcd.print("Offset: ");
  M5.Lcd.print(finalOffset);
  M5.Lcd.print(" | Scale: ");
  M5.Lcd.print(finalScale);
  M5.Lcd.println();
}


void calibrate()
{
  Serial.println("\n\nCALIBRATION\n===========");
  Serial.println("remove all weight from the loadcell");
  //  flush Serial input
  while (Serial.available()) Serial.read();

  Serial.println("and press enter\n");
  while (Serial.available() == 0);

  Serial.println("Determine zero weight offset");
  myScale.tare(20);  // average 20 measurements.
  uint32_t offset = myScale.get_offset();

  Serial.print("OFFSET: ");
  Serial.println(offset);
  Serial.println();

  offsets.push_back(offset);


  Serial.println("place a weight on the loadcell");
  //  flush Serial input
  while (Serial.available()) Serial.read();

  Serial.println("enter the weight in (whole) grams and press enter");
  uint32_t weight = 0;
  while (Serial.peek() != '\n')
  {
    if (Serial.available())
    {
      char ch = Serial.read();
      if (isdigit(ch))
      {
        weight *= 10;
        weight = weight + (ch - '0');
      }
    }
  }
  Serial.print("WEIGHT: ");
  Serial.println(weight);
  myScale.calibrate_scale(weight, 20);
  float scale = myScale.get_scale();

  Serial.print("SCALE:  ");
  Serial.println(scale, 6);

  scales.push_back(scale);

  Serial.print("\nuse scale.set_offset(");
  Serial.print(offset);
  Serial.print("); and scale.set_scale(");
  Serial.print(scale, 6);
  Serial.print(");\n");
  Serial.println("in the setup of your project");

  Serial.println("\n\n");
}

//  -- END OF FILE --
