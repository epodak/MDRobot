
// Teensy_MDRobot
// Written by James Belcher aka 'Fixstuff'
// Aug 2015

#include <Wire.h>
#include <I2C_Anything.h>
#include <Time.h>
#include <TimeLib.h>
#define HWSERIAL Serial1

//Variables
const byte MY_ADDRESS = 43;
volatile boolean haveData = false;
volatile byte fnum[16];
String string[5];
volatile byte dtotal;
byte buf [16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
int val;
byte in_buf [16];
int val2;
int val3;
int val4;
int led = 13;
int sw1 = 2;
int sw2 = 3;
int sw3 = 4;
int sw4 = 5;
byte digital_inputs = 0;
elapsedMillis readTemp;
elapsedMillis readAna;
elapsedMillis updateBuf;
elapsedMillis updateLED1;
elapsedMillis updateSerial;
elapsedMillis updateIO;
elapsedMillis updateIMU;
elapsedMillis updateTime;
elapsedMillis watchCheck;
boolean ledState;
int relay1 = 6;
int relay2 = 7;
int relay3 = 8;
int relay4 = 9;
int watchdog_tripped=1;
int wd_read;
int plc_ctr_org;

void setup() {
  // put your setup code here, to run once:
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);
  Wire.begin (MY_ADDRESS);
  Wire.onReceive (receiveEvent);
  Wire.onRequest (requestEvent);
  pinMode(led, OUTPUT);
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(sw3, INPUT);
  pinMode(sw4, INPUT);  
  analogReadRes(12);
  analogWriteResolution(12);
  HWSERIAL.begin(19200);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  //blink onboard LED when data arrives
  if (haveData)
    {
      //toggle LED after every 500ms
      if (updateLED1 >=500) {
        ledState = !ledState;
        digitalWrite(led, ledState);
        updateLED1=0;
      }
    haveData = false; 
    }  // end if haveData
    
    //Update Analog every 200ms
    if (readAna >= 100) {
      val = analogRead(0);
      val2 = analogRead(1);
      val3 = analogRead(2);
      val4 = analogRead(3);      
      readAna=0;
    }
    
    //Update Relay every 20ms. This only reads the buffer. the
    //pulse rate in the Codesys determines actual pulse.
    if (updateIO >= 20) {
      bitWrite(digital_inputs,0,digitalRead(sw1));
      bitWrite(digital_inputs,1,digitalRead(sw2));
      bitWrite(digital_inputs,2,digitalRead(sw3));
      bitWrite(digital_inputs,3,digitalRead(sw4));
      
      //turn off outputs if wd is bad
      if (watchdog_tripped == 0) {
        digitalWrite(relay1, bitRead(in_buf[1],0));
        digitalWrite(relay2, bitRead(in_buf[1],1));
        digitalWrite(relay3, bitRead(in_buf[1],2));
        digitalWrite(relay4, bitRead(in_buf[1],3));
      }
      else {
        digitalWrite(relay1, 0);
        digitalWrite(relay2, 0);
        digitalWrite(relay3, 0);
        digitalWrite(relay4, 0);
      } 
      updateIO=0;
    }
    
    //Update I2c output Buffers ever 20ms
    if (updateBuf >= 20) {
      buf[0]=lowByte(val);
      buf[1]=highByte(val);
      buf[2]=lowByte(val2);
      buf[3]=highByte(val2);
      buf[4]=lowByte(val3);
      buf[5]=highByte(val3);
      buf[6]=lowByte(val4);
      buf[7]=highByte(val4);
      buf[8]=digital_inputs;
      buf[9]=hour();
      buf[10]=minute();
      buf[11]=second();
      buf[12]=day();
      buf[13]=month();
      buf[14]=lowByte(year());
      buf[15]=highByte(year());   
    }
    
    //Every 500ms, send uTest1 to HW serial
    if (updateSerial >= 500) {
    HWSERIAL.print("Test1");
    HWSERIAL.write(13);                            //CR
    updateSerial=0;
    }

    //Check to make sure watchdog isn't tripped
    //save the current ctr value
    if (wd_read==0) {
      plc_ctr_org = in_buf[0];
      wd_read=1;
    }
    
    //verify it has changed in last 500ms
    if (watchCheck >= 500) {
      if (in_buf[0] == plc_ctr_org) {
        watchdog_tripped = 1;
      }
      else {
        watchdog_tripped = 0;
      }
       watchCheck=0;
      wd_read=0;
     } 
    
 } //main loop ends

// called by interrupt service routine when incoming data arrives
void receiveEvent (int howMany)
  {
  for (int i = 0; i < howMany; i++)
    {
    fnum[i] = Wire.read ();
    in_buf[i] = fnum[i];
    dtotal=howMany;
    haveData = true;
    }  // end of for loop
}  // end of receiveEvent

void requestEvent ()
  {
  Wire.write (buf, sizeof buf);   // send response
  }  // end of requestEvent

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}


