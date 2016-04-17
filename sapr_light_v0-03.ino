#include <FastLED.h>
#include <SoftwareSerial.h>

// для COM-порта
//SoftwareSerial mySerial(2, 1); // RX, TX
//bool receivingMode = false;  //сигнал начала приема получен, ждем данные (старт по 's')
//bool firstByteWait = false;  //ждем первый байт пакета, в нем зашит формат данных
int i; //счетчик
int i2;
bool ok; //для проверки на ошибки
byte c;
char cd;
int currByteReceive = 0;
int currByteState = 0;
int currPinName = 0;
int serialReadingMode = 0;
int serialReadingPosition = 0;
int serialReadingcolor = 0;
int LedPosition=0;
String Teststring="";
int TSP=0;
      //режимы приема:
      // 0 - приема нет
      // -1 - ждем первого байта с описанием формата пакета
      // 11 - ждем номер одиночного пина ('p')
      // 12 - ждем номер пина RGB ('r')
      // 13 - ждем номер пина с WS2812 ('w')
      // 1 - прием одиночного пина
      // 2 - прием RGB
      // 3 - прием картинки на WS2812

// для FastLED
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS_MAX  64

//int     LED_PIN = 5;
int     pinMapping[10]; // какой "условный пин" на какой ножке висит
                        
int     typePin[10]; // 1 - одиночный светодиод (или параллельная группа)
                     // 2 - RGB
                     // 3 - WS2812  количеством NUM_LEDS[i]
int     NUM_LEDS[10];


#define BRIGHTNESS  150
#define FRAMES_PER_SECOND 25

CRGB leds[NUM_LEDS_MAX];
int inpLed=0;

//-------------------------------------------------------------------------------
void Picture1(int FinpLed, int FnumLeds)
  {
      for (i=0; i<FnumLeds; i++)
      {
        leds[i]=FinpLed;
 //       if (FinpLed==0) {leds[i]=0;}
//        else {leds[i]= FinpLed*6+(FinpLed+i*8)*256+FinpLed*65536;}
      }
      
  }

//---------------------------------------------------------------------
//загрузка конфигурации пинов (в дальнейшем - заливать конфиг по serial)
void PinConfigure()
  {
    pinMapping[1] = 1;
    pinMapping[2] = 2;
    pinMapping[3] = 3;
    pinMapping[4] = 4;
    pinMapping[5] = 5;
    pinMapping[6] = 6;
    pinMapping[7] = 7;
    pinMapping[8] = 8;
    pinMapping[9] = 9;
        
    typePin[1] = 1; // 1 - одиночный светодиод
    typePin[2] = 1;
    typePin[3] = 1;
    typePin[4] = 1;
    typePin[5] = 1;
    typePin[6] = 1;
    typePin[7] = 1;
    typePin[8] = 1;
    typePin[9] = 3; // 3 - ws
    NUM_LEDS[1] = 1;
    NUM_LEDS[2] = 1;
    NUM_LEDS[3] = 1;
    NUM_LEDS[4] = 1;
    NUM_LEDS[5] = 1;
    NUM_LEDS[6] = 1;
    NUM_LEDS[7] = 1;
    NUM_LEDS[8] = 1;
    NUM_LEDS[9] = 16;
    pinMode(pinMapping[1], OUTPUT);
    pinMode(pinMapping[2], OUTPUT);
    pinMode(pinMapping[3], OUTPUT);
    pinMode(pinMapping[4], OUTPUT);
    pinMode(pinMapping[5], OUTPUT);
    pinMode(pinMapping[6], OUTPUT);
    pinMode(pinMapping[7], OUTPUT);
    pinMode(pinMapping[8], OUTPUT);
    pinMode(pinMapping[9], OUTPUT);
    
  }

//---------------------------------------------------------------------
//вывод содержимого архива leds
bool outputWS2812(int i)
  {
    if (typePin[i] != 3) {return(false);}
    else
    {
      FastLED.setBrightness( BRIGHTNESS );
      switch (pinMapping[i]) {
        /*case 1: FastLED.addLeds<CHIPSET, 1, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;
        case 2: FastLED.addLeds<CHIPSET, 2, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;
        case 3: FastLED.addLeds<CHIPSET, 3, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;
        case 4: FastLED.addLeds<CHIPSET, 4, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;
        case 5: FastLED.addLeds<CHIPSET, 5, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;
        case 6: FastLED.addLeds<CHIPSET, 6, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;
        case 7: FastLED.addLeds<CHIPSET, 7, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;
        case 8: FastLED.addLeds<CHIPSET, 8, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;*/
        case 9: FastLED.addLeds<CHIPSET, 9, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;
        /*case 10: FastLED.addLeds<CHIPSET, 10, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); break;
        case 12: FastLED.addLeds<CHIPSET, 12, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); FastLED.show(); break;
        case 11: FastLED.addLeds<CHIPSET, 11, COLOR_ORDER>(leds, NUM_LEDS[i]).setCorrection( TypicalLEDStrip ); FastLED.show(); break;*/
        
        default: break;
      } //case (i)

      FastLED.show(); // display this frame
      //FastLED.delay(1000 / FRAMES_PER_SECOND);
    }
  }

//----------------------------------------------------------------------------------------
void byteReceived()
{
  if ((serialReadingMode==0) && (cd=='s'))
    {
      //Serial.write(cd);
   //   Serial.println("serialReadingMode==0");
      serialReadingMode = -1;
      serialReadingPosition=0;
      return;
    }
  if (serialReadingMode!=0)
    {
      if (serialReadingMode==-1)
      {
      //    Serial.println("serialReadingMode==-1");
          switch (cd){
            case 'p': {serialReadingMode = 4; Serial.println(cd); break;} //режим приема номера одиночного пина
            case 'r': {serialReadingMode = 5; Serial.println(cd); break;}
            case 'w': {serialReadingMode = 6; Serial.println(cd); break;}
          }
      }

      else if (serialReadingMode==1) //режим приема номера одиночного пина, прием состояния пина 
      {
     //   Serial.println("serialReadingMode==1");
        currByteState = cd - '0'; //считываем состояние заданного пина
   //     Serial.println(currByteState);
        if (currByteState==0)
           digitalWrite(pinMapping[currPinName], LOW);
        if (currByteState==1)
           digitalWrite(pinMapping[currPinName], HIGH);
        serialReadingMode=0; //прием завершен, система возвращается в режим ожидания приема
      }

      else if (serialReadingMode==4) //режим приема номера одиночного пина, прием номера пина
        {
      //    Serial.println("serialReadingMode==4");
          currPinName = cd - '0'; //преобразование char to intrger
                                 //приняли и преодбразовали номер пина
     //     Serial.println(currPinName);
          serialReadingMode=1;
        }
        

      else if (serialReadingMode==5) //режим приема RGB, переключение в режим
      {
        serialReadingMode=0; //не реализовано, сбрасываем
      }

      else if (serialReadingMode==6) //режим приема WS, прием номера пина
      {
     //   Serial.println("serialReadingMode==6");
        currPinName = cd - '0'; //преобразование char to intrger
                                   //приняли и преодбразовали номер пина
      //  Serial.print(currPinName);
     //   Serial.print((char)currPinName);
        serialReadingMode=3; //режим приема WS, прием состояния
        inpLed=0;
        LedPosition=0;
      }

      else if (serialReadingMode==3) //режим приема WS, прием состояния
      {
      //  Serial.println("serialReadingMode==3");
        currByteState = 1;
        switch (cd) {  
          case '0':{currByteState=0; break;}
          case '1':{currByteState=1; break;}
          case '2':{currByteState=2; break;}
          case '3':{currByteState=3; break;}
          case '4':{currByteState=4; break;}
          case '5':{currByteState=5; break;}
          case '6':{currByteState=6; break;}
          case '7':{currByteState=7; break;}
          case '8':{currByteState=8; break;}
          case '9':{currByteState=9; break;}
          case 'a':{currByteState=10; break;}
          case 'b':{currByteState=11; break;}
          case 'c':{currByteState=12; break;}
          case 'd':{currByteState=13; break;}
          case 'e':{currByteState=14; break;}
          case 'f':{currByteState=15; break;}
          case 't':{Serial.write("c=t");
            ok = outputWS2812(currPinName);
            LedPosition=0;
            for (i=0; i<NUM_LEDS_MAX; i++)
              {
                leds[i]=0;
              }
            serialReadingMode=0;
            break;}
          default: currByteState=0; break;
          }
          
          serialReadingcolor=serialReadingcolor*16+currByteState;
        //  Serial.print("serialReadingPosition="); Serial.println(serialReadingPosition);
          serialReadingPosition++;
          if (serialReadingPosition>5)
          { 
          //  Serial.print(" if (serialReadingPosition>=6):  "); Serial.println(serialReadingPosition);
            Serial.print(" serialReadingcolor  "); Serial.println(serialReadingcolor);
            serialReadingPosition=0;
            leds[LedPosition]=serialReadingcolor;
            serialReadingcolor;
            LedPosition++;
            serialReadingcolor=0;
           }
        
      }//case 3:
     
    } //if (serialReadingMode!=0)
}        
   /*   if (Teststring.charAt(TSP)!='t'){
        
        if (Teststring.charAt(TSP)=='1') {currByteState=1;}
        if ( Teststring.charAt(TSP)=='2'){currByteState=2;}
        if ( Teststring.charAt(TSP)=='3'){currByteState=3;}
        if ( Teststring.charAt(TSP)=='4'){currByteState=4;}
        if ( Teststring.charAt(TSP)=='5'){currByteState=5;}
        if ( Teststring.charAt(TSP)=='6'){currByteState=6;}
        if ( Teststring.charAt(TSP)=='7'){currByteState=7;}
        if ( Teststring.charAt(TSP)=='8'){currByteState=8;}
        if ( Teststring.charAt(TSP)=='9'){currByteState=9;}
        if ( Teststring.charAt(TSP)=='a'){currByteState=10;}
        if ( Teststring.charAt(TSP)=='b'){currByteState=11;}
        if ( Teststring.charAt(TSP)=='c'){currByteState=12;}
        if ( Teststring.charAt(TSP)=='d'){currByteState=13;}
        if ( Teststring.charAt(TSP)=='e'){currByteState=14;}
        if ( Teststring.charAt(TSP)=='f'){currByteState=15;}
        if ( Teststring.charAt(TSP)=='0'){currByteState=0;}
*/        
        
 /*       


      } //if (Teststring.charAt(TSP)!='t')*/


void byteReceived1()
{
  Teststring=Teststring+cd;
  if (cd=='t') 
  {
    //TSP=0;
    for (TSP=0; TSP<Teststring.length(); TSP++)
    {
      cd=Teststring.charAt(TSP);
      //Serial.write( cd );
      byteReceived();
    }
    Serial.write("Fin rd");

    Teststring="";
    
  }
}

/*void textToHex(tx)
{
  
}*/
//-------------------------------------------------------------------------------------
void setup() {

  //delay(100); // sanity delay
  PinConfigure();  //конфигурация пинов
    
    // Start the hardware serial port
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
}

void loop()
{
  /*digitalWrite(9, HIGH);
  delay(200);
  digitalWrite(9, LOW);
  delay(200);*/
  //Picture1(5000, 5);
  //ok = outputWS2812(4);
  if (Serial.available())
  {
    //Serial.write(Serial.read());
    cd=(char)Serial.read();
    Serial.println(cd);
    if (cd!=0) byteReceived1();
  }
}
