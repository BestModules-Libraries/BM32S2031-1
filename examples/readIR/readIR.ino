/*****************************************************************
File:         readIR.ino
Description:  1.The delay of 0.5s after power-on is to ensure that the initialization of the BM32S2031_1 is completed.
              2.SoftwareSerial interface (BAUDRATE 9600)is used to communicate with BM32S2031_1.
              3.hardware Serial (BAUDRATE 9600) is used to communicate with Serial Monitor.
              4.Read  the BM32S2031_1 version number.
              5.Loop to obtain proximity sensing state. If the induction is successful, print the signal value
                on the serial monitor. If the reading of the signal value fails, print "communication fail".
connection method： rxPin:D2 txPin:D3
******************************************************************/
#include "BM32S2031-1.h"
BM32S2031_1     BMS31(2,3); //rxPin,txPin,Please comment out this line of code if you don't use SW Serial
//BM32S2031_1     BMS31(&Serial1); //Please uncomment out this line of code if you use HW Serial1 on BMduino
//BM32S2031_1     BMS31(&Serial2); //Please uncomment out this line of code if you use HW Serial2 on BMduino
//BM32S2031_1     BMS31(&Serial3); //Please uncomment out this line of code if you use HW Serial3 on BMduino
//BM32S2031_1     BMS31(&Serial4); //Please uncomment out this line of code if you use HW Serial4 on BMduino

int16_t FWVer = 0;
uint8_t irStatus = 0;
int16_t signalValue = 0;


void setup()
{ 
   BMS31.begin();
   Serial.begin(9600); //Set the communication rate between the serial monitor and BM32S2031_1 to 9600 baud rate    
   FWVer = BMS31.getFWVer();  //Gets the version number
   Serial.print("FWVer:");  
   Serial.println(FWVer,BIN);
}

void loop() 
{   
  irStatus = BMS31.getIRStatus();
  if(irStatus)  //Gets the proximity sensing state 0:not approach 1:approach
  {  
   signalValue = BMS31.readIRSignalValue(); //Gets the signal value    
   Serial.print("SignalValue:");  
   Serial.println(signalValue);
  }       
}
