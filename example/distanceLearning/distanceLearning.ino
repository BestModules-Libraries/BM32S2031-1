/*****************************************************************
File:         readIrStateAndControlMotor.ino
Description:  1.The delay of 0.5s after power-on is to ensure that the initialization of the BMS31M001 is completed.
              2.SoftwareSerial interface (BAUDRATE 9600)is used to communicate with BMS31M001.
              3.hardware Serial (BAUDRATE 9600) is used to communicate with Serial Monitor. 
              4.Distance learning mode (85cm) : Please place the paper 85cm away from the module .
                When the module finishes learning, the calibration result will be printed.
              5.Loop to obtain proximity sensing state. If the induction is successful, print the signal value
                on the serial monitor. If the reading of the signal value fails, print "communication fail". 
connection method： rxPin:D2 txPin:D3
******************************************************************/
#include "BM32S2031-1.h"
BM32S2031_1     BMS31(2,3); //rxPin,txPin,Please comment out this line of code if you don't use SW Serial
//BMS31M001     BMS31(&Serial1); //Please uncomment out this line of code if you use HW Serial1 on BMduino
//BMS31M001     BMS31(&Serial2); //Please uncomment out this line of code if you use HW Serial2 on BMduino
//BMS31M001     BMS31(&Serial3); //Please uncomment out this line of code if you use HW Serial3 on BMduino
//BMS31M001     BMS31(&Serial4); //Please uncomment out this line of code if you use HW Serial4 on BMduino

uint8_t irStatus = 0;
int16_t signalValue = 0;
unsigned long startTime=0;
uint8_t flag=1;
void setup() 
{  
    BMS31.begin();
    Serial.begin(9600);   //Set the communication rate between the serial monitor and BMS31M001 to 9600 baud rate
    startTime=millis();   //Get the distance learning start time
    flag = BMS31.distanceLearning();  //Distance learning success
   if( flag == 0 )  //Distance learning success
    {
     Serial.println("Distance learning success");
    }  
   if(flag == 1)  //Distance learning fail
    {
      Serial.println("Distance learning fail");
    }  
}

void loop() 
{
   if(BMS31.getIRStatus()) //Gets the proximity sensing state 0:not approach 1:approach
   {
    signalValue = BMS31.readIRSignalValue(); //Gets the signal value
    Serial.print(" SignalValue:");  
    Serial.println(signalValue);
   } 
}
