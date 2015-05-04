#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE(); 
//-----------------------------Матричная клавиатура---------------------
const byte ROWS = 4; //Количество столбцов матричной клавиатуры
const byte COLS = 4; //Количество строк матричной клавиатуры
byte rowPins[ROWS] = {8, 7, 6, 5}; //Пины столбцов
byte colPins[COLS] = {12, 11, 10, 9}; //Пины строк

byte buttonState[COLS][ROWS] = { //Массив состояния кнопок
  {1, 1, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 1}
};

byte kpdNote[COLS][ROWS] = { //Массив нот
  {60, 64, 68, 71},
  {61, 65, 69, 72},
  {62, 66, 70, 73},
  {63, 67, 71, 74}
};

//----------------------------------------------------------------------

//byte tmppin = 0;

byte val = 0;
byte dval = 0;

byte PrVal[8] = {0, 0, 0, 0, 0, 0, 0, 0};

long currenttime=0;
boolean b;
byte eval;
long ltime = 0;
 


void setup()
{
    MIDI.begin(1);  //Инициализация MIDI интерфейса
    attachInterrupt(1, enc, FALLING ); // Аппаратное прерывание. По спадающему фронту на пине 2 запускаем enc 

    pinMode(3, INPUT);//Вывод B энкодера
    pinMode(4, INPUT);//Неисп.

    pinMode(19, OUTPUT);//Пины A4 A5. Подключены в выводам A и B мультиплексора
    pinMode(18, OUTPUT);//-----------------------------------------------------

    pinMode(13, OUTPUT);//--Гасим встроенный светодиод--
    digitalWrite(13, LOW);//----------------------------
    
    for(byte i = 0; i < COLS; i++){      //--Конфигурируем строки мтрчн клвтр как выходы--
        pinMode(colPins[i], OUTPUT);     //--и подаём на них лог. 1-----------------------
        digitalWrite(colPins[i], HIGH);  // ----------------------------------------------
      }                                        
    for(byte i = 0; i < ROWS; i++){      //--Конфигурируем столбцы мтрчн клвтр как входы--------- 
        pinMode(rowPins[i], INPUT);      //--и включаем встроенные в мк подтягивающие резисторы--
        digitalWrite(rowPins[i], HIGH);  //------------------------------------------------------
      }      
}   


void loop()
{

  for(byte chn = 0; chn < 4; chn++)        //-Цикл чтения значений потенциометров-----------
   {
     set_mp_chn(chn);                      //--Задаём параметры мультиплексора--------------
     val=analogRead(0) / 8;                //--Считываем значение с канала X----------------
     if (abs(val-PrVal[chn]) > 5)          //--Если текущее значение отл. от прошлого-------
       {                                   //--больше чем на 5, то посылаем новое значение--
       MIDI.sendControlChange(chn,val,1);  
       PrVal[chn]=val;
       }
     val=analogRead(1) / 8;                //--Считываем значение с канала Y аналогично X----
     if (abs(val-PrVal[chn+4]) > 5)
       { 
       MIDI.sendControlChange(chn+4,val,1); 
       PrVal[chn+4]=val;
       }
   }                                       //-------------------------------------------------
    
  for(byte i = 0; i < COLS; i++)                                                                    //-Цикл чтения матричной клавиатуры----- 
    {
      digitalWrite(colPins[i], LOW);                                                                //--На считываем столбец выставляем 0---
      for(byte j = 0; j < ROWS; j++)                                                                //--Построчно считываем каждый столбец--
        {                                                                                           //--И при нажатой кнопке передаём ноту--
          dval=digitalRead(rowPins[j]);
          if ( dval == LOW && buttonState[i][j] == HIGH ) MIDI.sendNoteOn(kpdNote[j][i],127,1); 
          if ( dval == HIGH && buttonState[i][j] == LOW ) MIDI.sendNoteOff(kpdNote[j][i],127,1);
          buttonState[i][j] = dval;
        }
       digitalWrite(colPins[i], HIGH);  
    }                                                                                               //--------------------------------------     

}


void set_mp_chn(byte chn1) // Управление мультиплексором
{
  switch (chn1)
  {
    case 0: digitalWrite(18, LOW); digitalWrite(19, LOW); break; 
    case 1: digitalWrite(18, LOW); digitalWrite(19, HIGH); break;
    case 2: digitalWrite(18, HIGH); digitalWrite(19, LOW); break;
    case 3: digitalWrite(18, HIGH); digitalWrite(19, HIGH); break;
  }  
}

void enc() // Обработка энкодера
{
  currenttime=millis();
  if (abs(ltime-currenttime)>50) //  антидребезг
   {
    b=digitalRead(4);
    if (b == HIGH && eval<=122) eval=eval+5;
      else if (b == LOW && eval>=5) eval=eval-5;
    MIDI.sendControlChange(9,eval,1); 
    ltime = millis();
   }
   
}
