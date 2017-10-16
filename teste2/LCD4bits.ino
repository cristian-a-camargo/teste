
enum {
  LCD_D0=2,
  LCD_PIN_FIRST=6,
  LCD_D4=LCD_PIN_FIRST,
  LCD_D5=7,
  LCD_D6=8,
  LCD_D7=9,
  LCD_RS=10,
  LCD_PIN_D_LAST=LCD_RS,
  LCD_RW=11,
  LCD_E=12,
  LCD_PIN_LAST=LCD_E,
  LCD_PIN_MAX
};

#define LCD_CMD_CLEAR				0x01
#define LCD_CMD_HOME				0x02
#define LCD_CMD_ENTRYMODE			0x04
#define LCD_CMD_DISPLAY				0x08
#define LCD_CMD_SHIFT				0x10
#define LCD_CMD_FUNCTION			0x20
#define LCD_CMD_SETCGRAM			0x40
#define LCD_CMD_SETDDRAM			0x80

#define LCD_DRAM_LINE1			0x00
#define LCD_DRAM_LINE2			0x40

#define LCD_CMD_ENTRYMODE_INCPOS	0x02
#define LCD_CMD_ENTRYMODE_DECPOS	0x00
#define LCD_CMD_ENTRYMODE_SHIFTON	0x01
#define LCD_CMD_ENTRYMODE_SHIFTOFF	0x00

#define LCD_CMD_DISPLAY_DISPLAYON	0x04
#define LCD_CMD_DISPLAY_DISPLAYOFF	0x00
#define LCD_CMD_DISPLAY_CURSORON	0x02
#define LCD_CMD_DISPLAY_CURSOROFF	0x00
#define LCD_CMD_DISPLAY_BLINKON		0x01
#define LCD_CMD_DISPLAY_BLINKOFF	0x00

#define LCD_CMD_SHIFT_SHIFT		0x10
#define LCD_CMD_SHIFT_SHIFTDISPLAY	0x08
#define LCD_CMD_SHIFT_SHIFTCURSOR	0x00
#define LCD_CMD_SHIFT_SHIFTRIGHT	0x04
#define LCD_CMD_SHIFT_SHIFTLEFT		0x00

#define LCD_CMD_FUNCTION_8BIT		0x10
#define LCD_CMD_FUNCTION_4BIT		0x00
#define LCD_CMD_FUNCTION_1LINES		0x08
#define LCD_CMD_FUNCTION_2LINES		0x00
#define LCD_CMD_FUNCTION_FONT5X10	0x08
#define LCD_CMD_FUNCTION_FONT5X7	0x00

void LCDWaitBusy(void)
{
  int bf;

  for(int i = LCD_PIN_FIRST; i < LCD_PIN_D_LAST; i++) {
    pinMode(i, INPUT);
  }

  digitalWrite(LCD_RW, HIGH);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);

  while(1) {
    digitalWrite(LCD_E, LOW);
    digitalWrite(LCD_E, HIGH);
    bf=digitalRead(LCD_D7);
    digitalWrite(LCD_E, LOW);
    digitalWrite(LCD_E, HIGH);
    if( bf == LOW )
      break;
  }
  for(int i = LCD_PIN_FIRST; i < LCD_PIN_D_LAST; i++) {
    pinMode(i, OUTPUT);
  }
}

int LCDChar(unsigned char ch)
{
  int i, pin;

  digitalWrite(LCD_RW, LOW);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_E, HIGH);

  for(i=4, pin=LCD_D4; i < 8; i++,pin++) {
    if( (ch&(0x01<<i)) != 0 )
        digitalWrite(pin, HIGH);
    else
        digitalWrite(pin, LOW);
  }

  digitalWrite(LCD_E, LOW);
  digitalWrite(LCD_E, HIGH);

  for(i=0, pin=LCD_D4; i < 4; i++,pin++) {
    if( (ch&(0x01<<i)) != 0 )
        digitalWrite(pin, HIGH);
    else
        digitalWrite(pin, LOW);
  }

  digitalWrite(LCD_E, LOW);
  delay(1);
//  LCDWaitBusy();

  return 0;
}

/* Write 8 bits with D3-D0 as 0 */
int LCDCmd8(unsigned char cmd)
{
  int i, pin;

  digitalWrite(LCD_RW, LOW);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);

  for(i=4, pin=LCD_D4; i < 8; i++,pin++) {
    if( (cmd&(0x01<<i)) != 0 )
        digitalWrite(pin, HIGH);
    else
        digitalWrite(pin, LOW);
  }

  for(i=0, pin=LCD_D0; i < 4; i++,pin++) {
    digitalWrite(pin, LOW);
  }

  digitalWrite(LCD_E, LOW);
  delay(5);
  return 0;
}

int LCDCmd(unsigned char cmd)
{
  int i, pin;

  digitalWrite(LCD_RW, LOW);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);

  for(i=4, pin=LCD_D4; i < 8; i++,pin++) {
    if( (cmd&(0x01<<i)) != 0 )
        digitalWrite(pin, HIGH);
    else
        digitalWrite(pin, LOW);
  }

  digitalWrite(LCD_E, LOW);
  delay(2); 
  digitalWrite(LCD_E, HIGH);

  for(i=0, pin=LCD_D4; i < 4; i++,pin++) {
    if( (cmd&(0x01<<i)) != 0 )
        digitalWrite(pin, HIGH);
    else
        digitalWrite(pin, LOW);
  }

  digitalWrite(LCD_E, LOW);
  delay(5);
  return 0;
}

int LCDWriteString(int line, char *str)
{
  int i;

  if( line == 1 ) {
    LCDCmd(LCD_CMD_SETDDRAM|LCD_DRAM_LINE1);
  }
  if( line == 2 ) {
    LCDCmd(LCD_CMD_SETDDRAM|LCD_DRAM_LINE2);
  } else {
    LCDCmd( LCD_CMD_HOME );
  }

  for(i = 0; str[i] != '\0'; i++) {
    LCDChar(str[i]);
  }
}

int LCDSetup(void)
{
  delay(200);

  LCDCmd8( LCD_CMD_FUNCTION | LCD_CMD_FUNCTION_8BIT );
  LCDCmd8( LCD_CMD_FUNCTION | LCD_CMD_FUNCTION_8BIT );
  LCDCmd8( LCD_CMD_FUNCTION | LCD_CMD_FUNCTION_8BIT );

  LCDCmd8( LCD_CMD_FUNCTION | LCD_CMD_FUNCTION_4BIT );
  
  LCDCmd( LCD_CMD_FUNCTION | LCD_CMD_FUNCTION_4BIT | LCD_CMD_FUNCTION_2LINES | LCD_CMD_FUNCTION_FONT5X10 );

  LCDCmd( LCD_CMD_DISPLAY );
  LCDCmd( LCD_CMD_CLEAR );
  LCDCmd( LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_DISPLAYON | LCD_CMD_DISPLAY_CURSOROFF | LCD_CMD_DISPLAY_BLINKOFF );
  LCDCmd( LCD_CMD_ENTRYMODE | LCD_CMD_ENTRYMODE_INCPOS );

  return 0;
}

void setup()
{
  for(int i = LCD_PIN_FIRST; i < LCD_PIN_MAX; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  LCDSetup();
  delay(200);

  LCDWriteString(1, "Hello World!");
  LCDWriteString(2, "Segunda linha!");
  delay(2000);
  LCDWriteString(1, "Primeira linha!");
  delay(2000);
  LCDWriteString(2, "Segunda linha de novo!");

  delay(2000);

  for(int i=0; i <6; i++) {
    LCDCmd(LCD_CMD_SHIFT_SHIFT|LCD_CMD_SHIFT_SHIFTDISPLAY|LCD_CMD_SHIFT_SHIFTLEFT);
    delay(1000);
  }
  for(int i=0; i <6; i++) {
    LCDCmd(LCD_CMD_SHIFT_SHIFT|LCD_CMD_SHIFT_SHIFTDISPLAY|LCD_CMD_SHIFT_SHIFTRIGHT);
    delay(1000);
  }
}

void loop()
{
  delay(200);
}
