#ifndef LCD_H
#define LCD_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

//Templated class should be defined in header only
template<int ROWS, int COLS>
class LCD
{
  public:
    LiquidCrystal_I2C lcd;
    unsigned long lastUpdate;
    char lines[ROWS][COLS+1];

  public:
    LCD( uint8_t addr ):lcd(addr, COLS, ROWS)
    {
      lcd.begin();
      lcd.setBacklight( true );
      lcd.clear();

      for(int r = 0; r < ROWS; r++)
      {
        lines[r][0] = '\0';
      }
    }
    void show(const char* showLines[ROWS])
    {
      for(int r = 0; r < ROWS; r++)
      {
        if(strcmp(showLines[r], lines[r]) != 0)
        {
          copyForLcd( lines[r], showLines[r]);
          lcd.setCursor(0, r);
          lcd.print( lines[r]);
        }
      }
    }
    void copyForLcd(char* dest, const char* str)
    {
      int length = strlen(str);
      if(length > COLS)
      {
        Serial.print(F("BAD STR LENGTH! "));
        Serial.println(length);
        length = COLS;
      }
      strncpy(dest, str, COLS);
      for(int i = length; i < COLS; i++)
      {
        dest[i] = ' ';
      }
      dest[COLS] = '\0';
    }
};

#endif
