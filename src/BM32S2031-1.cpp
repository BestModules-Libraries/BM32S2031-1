/*****************************************************************
File:             BM32S2031-1.cpp
Author:           BEST MODULES CORP.
Description:      UART communication with the BM32S2031_1 and obtain the corresponding value  
Version:          V1.0.3   -- 2025-03-13
******************************************************************/
#include "BM32S2031-1.h"
/**********************************************************
Description: Constructor
Parameters:  *theSerial: Wire object if your board has more than one UART interface      
                         parameter range:
                                         BMduino UNO: &Serial、&Serial1、&Serial2、&Serial3、&Serial4
                                         Arduino UNO：&Serial
Return:          
Others:     
**********************************************************/
BM32S2031_1::BM32S2031_1(HardwareSerial *theSerial)
{
     _softSerial = NULL;  
     _hardSerial = theSerial;
}
/**********************************************************
Description: Constructor
Parameters: rxPin: Receiver pin of the UART
            txPin: Send signal pin of UART         
Return:          
Others:   
**********************************************************/
BM32S2031_1::BM32S2031_1(uint8_t rxPin,uint8_t txPin)
{
    _hardSerial = NULL;
    _rxPin = rxPin;
    _txPin = txPin;
    _softSerial = new SoftwareSerial(_rxPin,_txPin);
}

/**********************************************************
Description: Module Initial
Parameters:  baudRate: Module communication baud rate(Unique 9600bps)        
Return:          
Others:   If the hardware UART is initialized, the _softSerial 
          pointer is null, otherwise it is non-null       
**********************************************************/
void BM32S2031_1::begin(uint16_t baud)
{
    if(_softSerial != NULL)
    {
        _softSerial->begin(baud);
        getFWVer(); //  The UART mode is displayed (Any instruction)
    }
    else
    {
        _hardSerial->begin(baud);
        getFWVer();  //  The UART mode is displayed (Any instruction)
    }
}


/**********************************************************
Description: Get IR Induction state
Parameters:         
Return:      irStatus: 1:approach   0:not approach    
Others:      
**********************************************************/
uint8_t BM32S2031_1::getIRStatus()
{
    uint8_t sendBuf[5] = {0x55, 0x80, 0x02, 0x01,0xD8};
    uint8_t irStatus = 0;
    uint8_t buff[6] = {0};
    writeBytes(sendBuf,5);
    if(readBytes(buff,6)== CHECK_OK)
    {
     irStatus = buff[4];
    }
    delay(10);
    return irStatus;
}

/**********************************************************
Description: Read  signal value
Parameters:         
Return:      signalValue: irEnv - irRef     
Others:      
**********************************************************/
int16_t BM32S2031_1::readIRSignalValue()
{
    int16_t signalValue = 0;
    uint8_t irRef = 0;
    uint8_t irEnv = 0;
    irRef = readIRRef();
    irEnv = readIREnv();
    signalValue = irEnv - irRef;
    return signalValue;
}

/**********************************************************
Description: Enter distance learning mode
Parameters:        
Return:      0:distance Learn Success 
             1:distance Learn Fail       
Others:      Place the obstacle above BM32S2031_1 at the distance you want to learn. 
             The learning is  complete when the OUT pin level of BM32S2031_1 changes.
**********************************************************/
uint8_t BM32S2031_1::distanceLearning()
{
    uint8_t sendBuf[3] = {0x55, 0x19, 0x6E};
    uint8_t buff[3] = {0};
    writeBytes(sendBuf,3); 
    if(readBytes(buff,3)== CHECK_OK)
    {
      if(buff[1]== 0x7F)
      {
       if(readBytes(buff,3,3000)== CHECK_OK)
       {
        if(buff[1]== 0x7D)
        {
          delay(10);
          return SUCCESS ;
        }
         delay(10);
         return FAIL ;
       }
      }
    }
  delay(10);
  return FAIL ;
}

/**********************************************************
Description: Module reset
Parameters:        
Return:      0:Success 1:Fail       
Others:    
**********************************************************/
uint8_t BM32S2031_1::reset()
{
    uint8_t sendBuf[3] = {0x55, 0x10, 0x65};
    uint8_t buff[3] = {0};
    writeBytes(sendBuf,3); 
    if(readBytes(buff,3)== CHECK_OK)
    {
     if(buff[1]== 0x7f)
     {
       delay(70);
       return SUCCESS;
     }
    }
    delay(70);
    return FAIL ;
}

/**********************************************************
Description: Save function parameters to EEPROM
Parameters:         
Return:      0:Success 1:Fail     
Others:      Writes the current parameter to EEPROM
**********************************************************/
uint8_t BM32S2031_1::saveSettings()
{
  uint8_t sendBuf[3] = {0x55, 0x1A, 0x6F}; 
  uint8_t buff[3] = {0};
  writeBytes(sendBuf,3);
  delay(40);
  if(readBytes(buff,3)== CHECK_OK)
   {
    if(buff[1]== 0x7f)
    {
     delay(10);
     return SUCCESS;
    }
   }
   delay(10);
   return FAIL ;
}

/**********************************************************
Description: Assigns values to function parameters by reading 
             values from EEPROM
Parameters:         
Return:      0:Success 1:Fail     
Others:      Read function parameters from EEPROM
**********************************************************/
uint8_t BM32S2031_1::restoreSettings()
{
   uint8_t sendBuf[3] = {0x55, 0x1C, 0x71}; 
   uint8_t buff[3] = {0};
   writeBytes(sendBuf,3);
   if(readBytes(buff,3)== CHECK_OK)
   {
    if(buff[1]== 0x7f)
    {
     delay(10);
     return SUCCESS;
    }
   }
   delay(10);
   return FAIL ; 
}

/**********************************************************
Description: Get the version information
Parameters:         
Return:      ver = ver_highByte<<8+ver_lowByte     
Others:      EX. //High byte     LOW byte
                 //0b00000 0001  0000 0000(version 1.0)
**********************************************************/
int16_t BM32S2031_1::getFWVer()
{
    uint8_t sendBuf1[5] = {0x55, 0x80, 0x00, 0x01, 0xD6};
    uint8_t sendBuf2[5] = {0x55, 0x80, 0x01, 0x01, 0xD7};
    uint8_t buff[6] = {0};
    int16_t ver;
    uint8_t ver_l = 0;
    uint8_t ver_h = 0;
    writeBytes(sendBuf1,5);
    if(readBytes(buff,6)== CHECK_OK)
    {
     ver_l = buff[4];
    }
    delay(10);
    writeBytes(sendBuf2,5);
    if(readBytes(buff,6)== CHECK_OK)
    {
     ver_h = buff[4];
    }
    ver = ver_l + (ver_h<<8);
    delay(10);
    return ver;
}

/**********************************************************
Description: Get IR trigger threshold
Parameters:         
Return:      threshold: Trigger threshold(16~180)   
Others:      
**********************************************************/
uint8_t BM32S2031_1::getIRThreshold()
{
    uint8_t sendBuf[5] = {0x55, 0x80, 0x08, 0x01,0xDE};
    uint8_t threshold = 0;
    uint8_t buff[6] = {0};
    writeBytes(sendBuf,5);
    if(readBytes(buff,6)== CHECK_OK)
    {
     threshold = buff[4];
    }
    delay(10);
    return threshold;
}

/**********************************************************
Description: Get IR debounce value
Parameters:         
Return:      debounce: Number of debounces(0~15)   
Others:      
**********************************************************/
uint8_t BM32S2031_1::getIRDebounce()
{
  uint8_t sendBuf[5] = {0x55, 0x80, 0x09, 0x01,0xDF};
  uint8_t debounce = 0;
  uint8_t buff[6] = {0};
  writeBytes(sendBuf,5);
  if(readBytes(buff,6)== CHECK_OK)
   {
    debounce = buff[4];
   }
  delay(10);
  return debounce;
}

/**********************************************************
Description: Get IR response time 
Parameters:         
Return:      responseTime: 
                          0x00:SPEED_8_MS       
                          0x01:SPEED_16_MS      
                          0x02:SPEED_32_MS       
                          0x03:SPEED_64_MS      
                          0x04:SPEED_128_MS      
                          0x05:SPEED_256_MS      
                          0x06:SPEED_500_MS
                          0x07:SPEED_1_S  
                          0x08:SPEED_FASTMODE    
Others:      
**********************************************************/
uint8_t BM32S2031_1::getIRResponseTime()
{
    uint8_t sendBuf[5] = {0x55, 0x80, 0x0A, 0x01,0xE0};
    uint8_t responseTime = 0;
    uint8_t buff[6] = {0};
    writeBytes(sendBuf,5);
    if(readBytes(buff,6)== CHECK_OK)
    {
     responseTime = buff[4];
    }
    delay(10);
    return responseTime;
}

/**********************************************************
Description: Get IR output time
Parameters:         
Return:      outputtime: 0X00~0XFF 
                         0X00: 0 seconds
                         0x01: 1 seconds
                          ...
                         0xFF: 255 seconds   
Others:      When object is detected, delay the output. 
             (Only valid in general high and low level output mode)   
**********************************************************/
uint8_t BM32S2031_1::getIROutputTime()
{
    uint8_t sendBuf[5] = {0x55, 0x80, 0x0B, 0x01,0xE1};
    uint8_t outputTime;
    uint8_t buff[6] = {0};
    writeBytes(sendBuf,5);
    if(readBytes(buff,6)== CHECK_OK)
    {
     outputTime = buff[4];
    }
    delay(10);
    return outputTime;
}

/**********************************************************
Description: Get Mode function setting
Parameters:         
Return:       Mode :
                    Bit4~0 Setting is invalid
                    Bit5: Output mode selection 
                          0:High/Low(High and low level output mode)
                           1:Pulse/Toggle(Pulse switching mode) 
                    Bit6: Pulse/Toggle mode selection(only valid if Bit 5=1) 
                          0:Pulse mode
                          1:Toogle mode 
                    Bit7: Output level setting
                          0:Normal high level. Active low level
                          1:Normal low level. Active high level     
Others:      Pulse mode: Output 200μs per second as the object approaches
             Toogle mode: Object close to open output, object left close again output  
**********************************************************/
uint8_t BM32S2031_1::getMode()
{
    uint8_t sendBuf[5] = {0x55, 0x80, 0x0C, 0x01,0xE2};
    uint8_t mode;
    uint8_t buff[6] = {0};
    writeBytes(sendBuf,5);
    if(readBytes(buff,6)== CHECK_OK)
    {
     mode = buff[4];
    }
    delay(10);
    return mode;
}

/**********************************************************
Description: Get IR emission current value 
Parameters:         
Return:      current: 
                      0~ 63,5 mA/step, first order amplification  
                      64~ 127,5 mA/step, two-stage amplification  
                      The calculation formula of current value is as follows:  
                      Dn >= 63, IEmmision = (Dn - 64) x 5 + 5  
                      Dn <= 63 or less, IEmmision = Dn x 5 + 5        
Others:      
**********************************************************/
uint8_t BM32S2031_1::getIRCurrent()
{
  uint8_t sendBuf[5] = {0x55, 0x80, 0x0D, 0x01,0xE3};
  uint8_t current = 0;
  uint8_t buff[6] = {0};
  writeBytes(sendBuf,5);
  if(readBytes(buff,6)== CHECK_OK)
   {
    current = buff[4];
   }
  delay(10);
  return current;
}

/**********************************************************
Description: Set IR trigger threshold
Parameters:  threshold: Trigger threshold
                        parameter range:16~180(Default 16)      
Return:      0:Success 1:Fail    
Others:      
**********************************************************/
uint8_t BM32S2031_1::setIRThreshold(uint8_t  threshold)
{
  uint8_t sendBuf[6] = {0x55, 0xC0, 0x08, 0x01, 0x00, 0x00};
    uint8_t buff[3] = {0};
    sendBuf[4] = threshold;
    sendBuf[5] = 30+threshold;
    writeBytes(sendBuf,6); 
    if(readBytes(buff,3)== CHECK_OK)
    {
     if(buff[1]== 0x7f)
     {
       delay(10);
       return SUCCESS;
     }
    }
    delay(10);
    return FAIL ;
}

/**********************************************************
Description:  Set IR debounce value
Parameters:  debounce: Number of debounces
                       parameter range:0~15(Default 0)      
Return:      0:Success 1:Fail    
Others:      
**********************************************************/
uint8_t BM32S2031_1::setIRDebounce(uint8_t  debounce)
{
  uint8_t sendBuf[6] = {0x55, 0xC0, 0x09, 0x01, 0x00, 0x00};
  uint8_t buff[3] = {0};
  sendBuf[4] = debounce;
  sendBuf[5] = 31+debounce;
  writeBytes(sendBuf,6);
  if(readBytes(buff,3)== CHECK_OK)
   {
    if(buff[1]== 0x7f)
    {
     delay(10);
     return SUCCESS;
    }
   }
   delay(10);
   return FAIL ;
}

/**********************************************************
Description: Set IR response time
Parameters:  response time: 0x00~0x08 (Default 0x06)
             Optional:
                      0x00:SPEED_8_MS       
                      0x01:SPEED_16_MS      
                      0x02:SPEED_32_MS       
                      0x03:SPEED_64_MS      
                      0x04:SPEED_128_MS      
                      0x05:SPEED_256_MS      
                      0x06:SPEED_500_MS
                      0x07:SPEED_1_S  
                      0x08:SPEED_FASTMODE        
Return:      0:Success 1:Fail 
Others:      
**********************************************************/
uint8_t BM32S2031_1::setIRResponseTime(uint8_t  responseTime)
{
  uint8_t sendBuf[6] = {0x55, 0xC0, 0x0A, 0x01, 0x00, 0x00};
  uint8_t buff[3] = {0};
  sendBuf[4] = responseTime;
  sendBuf[5] = 32+responseTime;
  writeBytes(sendBuf,6);
  if(readBytes(buff,3)== CHECK_OK)
  {
   if(buff[1]== 0x7f)
   {
     delay(10);
     return SUCCESS;
   }
  }
  delay(10);
  return FAIL ;
}

/**********************************************************
Description: Set IR output time
Parameters:  outputTime:  0X00~0XFF (Default 0x00)
                          0X00: 0 seconds
                          0x01: 1 seconds
                          ...
                          0xFF: 255 seconds       
Return:      0:Success 1:Fail 
Others:      When object is detected, delay the output. 
            (Only valid in general high and low level output mode)
**********************************************************/
uint8_t BM32S2031_1::setIROutputTime(uint8_t outputTime)
{ 
  uint8_t sendBuf[6] = {0x55, 0xC0, 0x0B, 0x01, 0x00, 0x00};
  uint8_t buff[3] = {0};
  sendBuf[4] = outputTime;
  sendBuf[5] = 33+outputTime;
  writeBytes(sendBuf,6);
 if(readBytes(buff,3)== CHECK_OK)
  {
   if(buff[1]== 0x7f)
   {
     delay(10);
     return SUCCESS;
   }
  }
  delay(10);
  return FAIL ;
}

/**********************************************************
Description: Set Mode function setting
Parameters:  mode: 0b00000000~0b11111111 (Default 0b00010011) 
                  Bit4~0 Setting is invalid
                  Bit5: Output mode selection 
                        0:High/Low(High and low level output mode)
                        1:Pulse/Toggle(Pulse switching mode) 
                  Bit6: Pulse/Toggle mode selection(only valid if Bit 5=1) 
                        0:Pulse mode
                        1:Toogle mode 
                  Bit7: Output level setting
                        0:Normal high level. Active low level
                        1:Normal low level. Active high level       
Return:      0:Success 1:Fail 
Others:      Pulse mode: Output 200μs per second as the object approaches
             Toogle mode: Object close to open output, object left close again output
**********************************************************/
uint8_t BM32S2031_1::setMode(uint8_t mode)
{
  uint8_t sendBuf[6] = {0x55, 0xC0, 0x0C, 0x01, 0x00, 0x00};
  uint8_t buff[3] = {0};
  sendBuf[4] = mode;
  sendBuf[5] = 34+mode;
  writeBytes(sendBuf,6);
 if(readBytes(buff,3)== CHECK_OK)
  {
   if(buff[1]== 0x7f)
   {
     delay(10);
     return SUCCESS;
   }
  }
  delay(10);
  return FAIL ;
}

/**********************************************************
Description: Set IR emission current value
Parameters:  current:Emission current parameters  
                  parameter range: 
                                  0~ 63,5 mA/step, first order amplification  
                                  64~ 127,5 mA/step, two-stage amplification  
                                  The calculation formula of current value is as follows:  
                                  Dn >= 63, IEmmision = (Dn - 64) x 5 + 5  
                                  Dn <= 63 or less, IEmmision = Dn x 5 + 5       
Return:      0:Success 1:Fail            
Others:      
**********************************************************/
uint8_t BM32S2031_1::setIRCurrent(uint8_t  current)
{ 
  uint8_t sendBuf[6] = {0x55, 0xC0, 0x0D, 0x01, 0x00, 0x00};
  uint8_t buff[3] = {0};
  sendBuf[4] = current;
  sendBuf[5] = 35+current;
  writeBytes(sendBuf,6);
  if(readBytes(buff,3)== CHECK_OK)
  {
   if(buff[1]== 0x7f)
   {
     delay(10);
     return SUCCESS;
   }
  }
  delay(10);
  return FAIL ;
}



/**********************************************************
Description: Get reference value 
Parameters:         
Return:      IrRef:Proximity sensing reference value. (Sensing value when IR LED is not on)      
Others:      
**********************************************************/
uint8_t BM32S2031_1::readIRRef()
{
  uint8_t sendBuf[5] = {0x55, 0x80, 0x03, 0x01,0xD9};
  uint8_t IrRef = 0;
  uint8_t buff[6] = {0};
  writeBytes(sendBuf,5);
  if(readBytes(buff,6)== CHECK_OK)
  {
   IrRef = buff[4];
  }
  delay(10);
  return IrRef;
}

/**********************************************************
Description: Get environmental value 
Parameters:         
Return:      IREnv:Proximity sensing environmental values. (Sensing value when IR LED is turned on )     
Others:      
**********************************************************/
uint8_t BM32S2031_1::readIREnv()
{
  uint8_t sendBuf[5] = {0x55, 0x80, 0x04, 0x01,0xDA};
  uint8_t IREnv =  0;
  uint8_t buff[6] = {0};
  writeBytes(sendBuf,5);
 if(readBytes(buff,6)== CHECK_OK)
   {
    IREnv = buff[4];
   }
  delay(10);
  return IREnv;
}

/**********************************************************
Description: writeBytes
Parameters:  wbuf[]:Variables for storing Data to be sent
             wlen:Length of data sent  
Return:   
Others:
**********************************************************/
void BM32S2031_1::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
  /* Select SoftwareSerial Interface */
  if (_softSerial != NULL)
  {
    while (_softSerial->available() > 0)
    {
      _softSerial->read();
    }
    _softSerial->write(wbuf, wlen);
  }
  /* Select HardwareSerial Interface */
  else
  {
    while (_hardSerial->available() > 0)
    {
      _hardSerial->read();
    }
    _hardSerial->write(wbuf, wlen);
  }
}

/**********************************************************
Description: readBytes
Parameters:  rbuf[]:Variables for storing Data to be obtained
             rlen:Length of data to be obtained
Return:   
Others:
**********************************************************/
uint8_t BM32S2031_1::readBytes(uint8_t rbuf[], uint8_t rlen, uint16_t timeOut)
{
  uint8_t i = 0, delayCnt = 0, checkSum = 0;
/* Select SoftwareSerial Interface */
  if (_softSerial != NULL)
  {
    for (i = 0; i < rlen; i++)
    {
      delayCnt = 0;
      while (_softSerial->available() == 0)
      {
        if (delayCnt > timeOut)
        {
          return TIMEOUT_ERROR; // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i] = _softSerial->read();
    }
  }
/* Select HardwareSerial Interface */
  else
  {
    for (i = 0; i < rlen; i++)
    {
      delayCnt = 0;
      while (_hardSerial->available() == 0)
      {
        if (delayCnt > timeOut)
        {
          return TIMEOUT_ERROR; // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i] = _hardSerial->read();
    }
  }

  /* check Sum */
  for (i = 0; i < (rlen - 1); i++)
  {
    checkSum += rbuf[i];
  }
  if (checkSum == rbuf[rlen - 1])
  {
    return CHECK_OK; // Check correct
  }
  else
  {
    return CHECK_ERROR; // Check error
  }
}
