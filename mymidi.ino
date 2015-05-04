#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE(); 

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {12, 11, 10, 9}; //connect to the column pinouts of the keypad
byte tmppin = 0;

byte val = 0;
byte dval = 0;
byte buttonState[COLS][ROWS] = {
  {1, 1, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 1}
};

byte kpdNote[COLS][ROWS] = {
  {60, 64, 68, 71},
  {61, 65, 69, 72},
  {62, 66, 70, 73},
  {63, 67, 71, 74}
};

byte PrVal[8] = {0, 0, 0, 0, 0, 0, 0, 0};

long currenttime=0;
boolean b;
byte eval;
long ltime = 0;
 


void setup()
{
    MIDI.begin(1);
    attachInterrupt(1, enc, FALLING ); 
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(13, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(18, OUTPUT);
    digitalWrite(13, LOW);
    for(byte i = 0; i < COLS; i++){
        pinMode(colPins[i], OUTPUT);
        digitalWrite(colPins[i], HIGH); 
      }
    for(byte i = 0; i < ROWS; i++){
        pinMode(rowPins[i], INPUT);
        digitalWrite(rowPins[i], HIGH);
      }      
}   


void loop()
{

  for(byte chn = 0; chn < 4; chn++)
   {
     set_mp_chn(chn);
     val=analogRead(0) / 8;
     if (abs(val-PrVal[chn]) > 5) 
       {
       MIDI.sendControlChange(chn,val,1); 
       PrVal[chn]=val;
       }
     val=analogRead(1) / 8;
     if (abs(val-PrVal[chn+4]) > 5)
       { 
       MIDI.sendControlChange(chn+4,val,1); 
       PrVal[chn+4]=val;
       }
   }        
    
  for(byte i = 0; i < COLS; i++)
    {
      digitalWrite(colPins[i], LOW);
      for(byte j = 0; j < ROWS; j++)
        {
          dval=digitalRead(rowPins[j]);
          if ( dval == LOW && buttonState[i][j] == HIGH ) MIDI.sendNoteOn(kpdNote[j][i],127,1); 
          if ( dval == HIGH && buttonState[i][j] == LOW ) MIDI.sendNoteOff(kpdNote[j][i],127,1);
          buttonState[i][j] = dval;
        }
       digitalWrite(colPins[i], HIGH);  
    }

}


void set_mp_chn(byte chn1)
{
  switch (chn1)
  {
    case 0: digitalWrite(18, LOW); digitalWrite(19, LOW); break; //PORTC = PORTC | (0 << PORTC5); PORTC = PORTC | (0 << PORTC6); break;
    case 1: digitalWrite(18, LOW); digitalWrite(19, HIGH); break;//PORTC = PORTC | (0 << PORTC5); PORTC = PORTC | (1 << PORTC6); break;
    case 2: digitalWrite(18, HIGH); digitalWrite(19, LOW); break;//PORTC = PORTC | (1 << PORTC5); PORTC = PORTC | (0 << PORTC6); break;
    case 3: digitalWrite(18, HIGH); digitalWrite(19, HIGH); break;//PORTC = PORTC | (1 << PORTC5); PORTC = PORTC | (1 << PORTC6); break;
  }  
}

void enc()
{
  currenttime=millis();
  if (abs(ltime-currenttime)>50)
   {
    b=digitalRead(4);
    if (b == HIGH && eval<=122) eval=eval+5;
      else if (b == LOW && eval>=5) eval=eval-5;
    MIDI.sendControlChange(9,eval,1); 
    ltime = millis();
   }
   
}
