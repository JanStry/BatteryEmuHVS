//CAN lib
#include <mcp_can.h>
#include <SPI.h>
//timer lib
#include "SandTimer.h"
//average lib
#include "Arduino.h"
#include <AverageValue.h>
//MODBUS lib
#include <ModbusRtu.h> 
//AltSoftSerial lib
#include <AltSoftSerial.h>

//average
const long MAX_VALUES_NUM = 10;   // Number of values to calculate average current reading
AverageValue<long> averageValue(MAX_VALUES_NUM);
float rawmeasurement = 0;
//BYD INITIAL MESSAGE
unsigned char CANData250[8] = {0x03, 0x29, 0x00, 0x66, 0x19, 0x19, 0x02, 0x09};   //0-1 FW version , Capacity kWh byte4&5 (example 24kWh = 240)
unsigned char CANData290[8] = {0x06, 0x37, 0x10, 0xD9, 0x00, 0x00, 0x00, 0x00};
unsigned char CANData2D0[8] = {0x00, 0x42, 0x59, 0x44, 0x00, 0x00, 0x00, 0x00};    //BYD
unsigned char CANData3D0[8] = {0x00, 0x42, 0x61, 0x74, 0x74, 0x65, 0x72, 0x79};    //Battery  CAN_frame BYD_3D0_0
unsigned char CANData3D0_1[8] = {0x01, 0x2D, 0x42, 0x6F, 0x78, 0x20, 0x50, 0x72};    //-Box Pr  CAN_frame BYD_3D0_1
unsigned char CANData3D0_2[8] = {0x02, 0x65, 0x6D, 0x69, 0x75, 0x6D, 0x20, 0x48};    //emium H  CAN_frame BYD_3D0_2
unsigned char CANData3D0_3[8] = {0x03, 0x56, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00};    //VS       CAN_frame BYD_3D0_3 
//Actual content messages BYD
unsigned char CANData110[8] = {0x01, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};     //send_limits
unsigned char CANData150[8] = {0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00};     //send_states
unsigned char CANData190[8] = {0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};     //send alarms
unsigned char CANData1D0[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x08};     //send_battery_info
unsigned char CANData210[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};     //send_cell_info

//CAN RECEIVER DATA
long unsigned int rxId;  //received id for request
long unsigned int SolisReq = 0x151; //inverter request id
unsigned char len = 0;  //received message lenght
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string
//CAN DEFFINITION
#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10

// MODBUS 
#define slaveID 4           //modbus slave ID
#define TXenable 3          //TXenable pin 
// data array for modbus network sharing
uint16_t au16data[16]  = {
  0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//MODBUS DEFINITIONS
AltSoftSerial altSerial;
Modbus slave(slaveID,altSerial,TXenable); // SoftSerial port + pin3 for TX enable

//SETUP TIMERS
SandTimer timer_2s;
SandTimer timer_10s;
SandTimer timer_60s;
//INT
int INITIALMSG = 1;                        //INITIAL MESSAGE MARKER. //always 0 in run. used for debug
int CanError = 0;
// Global battery stat variables 
//float CellMax = 5;                 // Default value > max possible cell votlage
//float CellMin = 0; 
//float Celldiff = 0;
float PackVoltagef = 0; 
float PackCurrentf = 0;
float RemainCapacityf = 0;         // Optional
float Temp = 0;
float SOC = 0; 
//float Temp_probe_2 = 0;            // Optional
//bool chargeFet = 0;                // Optional
//bool dischargeFet = 0;             // Optional
//bool AFEerror = 0;                 // Optional

// ****** This limits are to suit this battery *******
float batteryChargeVoltage = 214.4;   // Max battery voltage
float batteryDischargeVoltage = 170;  // Min battery voltage
float ChargeCurrentLimit = 5;       // Max charge current
float DischargeCurrentLimit = 5;    // Min discharge current
float StateOfHealth = 100;            // SOH
float MaxCapacity = 25;        // Max capacity x10 (kW x10)



void setup()
{
  Serial.begin(9600);
  altSerial.begin(9600);

  slave.start();  //start modbus 
  
  // Initialize MCP2515 running at 8MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  
  Serial.println("MCP2515 Going into listen mode...");
}

void loop()
{

  if(INITIALMSG == 0){
    if(!digitalRead(CAN0_INT))                  // If CAN0_INT pin is low, read receive buffer
    {
     CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
     if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
       sprintf(msgString, "ExtID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
     else
        sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
        if(rxId == SolisReq){
          Serial.println("Inverter request received");
          INITIALMSG = 1;
        }
      Serial.print(msgString);
  
      if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
        sprintf(msgString, "REQ FRAME");
        Serial.print(msgString);
     } else {
        for(byte i = 0; i<len; i++){
         sprintf(msgString, " 0x%.2X", rxBuf[i]);
         Serial.print(msgString);
      }
      }
        
      Serial.println();
    }
  }
  if(INITIALMSG == 1)
  {
  
    Serial.println("Enter send mode");
    delay(480);
    Serial.println("Sending initial message");

    CAN0.sendMsgBuf(0x250, 0, 8, CANData250);   //0-1 FW version , Capacity kWh byte4&5 (example 24kWh = 240)
    delay(5);
    //memset(0x250, 0, 8);

    CAN0.sendMsgBuf(0x290, 0, 8, CANData290);   // ????
    delay(5);
    //memset(0x290, 0, 8);

    CAN0.sendMsgBuf(0x2D0, 0, 8, CANData2D0);   //BYD
    delay(5);
    //memset(0x2D0, 0, 8);

    CAN0.sendMsgBuf(0x3D0, 0, 8, CANData3D0);   //Battery  CAN_frame BYD_3D0_0
    //delay(5);
    
    memset(0x3D0, 0, 8);
    //delay(5);
    
    CAN0.sendMsgBuf(0x3D0, 0, 8, CANData3D0_1);   //-Box Pr  CAN_frame BYD_3D0_1
    delay(5);
    //memset(0x3D0, 0, 8);

    delay(5);
    CAN0.sendMsgBuf(0x3D0, 0, 8, CANData3D0_2);   //emium H  CAN_frame BYD_3D0_2
    delay(5);
    //memset(0x3D0, 0, 8);

    delay(5);
    byte sndStat = CAN0.sendMsgBuf(0x3D0, 0, 8, CANData3D0_3);   //VS       CAN_frame BYD_3D0_3 
    delay(5);
    //memset(0x3D0, 0, 8);
      if(sndStat == CAN_OK){
      Serial.println("Message Sent Successfully!");
      } else {
      Serial.println("Error Sending Message...");
      }
    INITIALMSG = 2;
    delay(50);
  }
  if(INITIALMSG == 2)
  {
   //Serial.println("");
   timer_2s.start(1900);     //start timer 2sec
   timer_10s.start(9900);    //start timer 2sec
   timer_60s.start(59900);   //start timer 2sec

   if (timer_2s.finished())
   {
    //Serial.println("timer 2s finished");
    Serial.print("SoC: ");
    Serial.print(SOC);
    Serial.print("   Volts: ");
    Serial.print(PackVoltagef,1);
    Serial.print("   Current: ");
    Serial.print(PackCurrentf,1);
    Serial.print("   Temp: ");
    Serial.println(Temp,1);
    //Serial.println("");
    CAN2S();
    timer_2s.startOver(); 
   }

  if (timer_10s.finished())
   {
    //Serial.println("timer 10s finished");
    CAN10S();
    timer_10s.startOver(); 
   }

   if (timer_60s.finished())
   {
    //timer_10s.startOver(); 
    //timer_2s.startOver(); 
    //Serial.println("timer 60s finished");
    CAN60S();
    timer_60s.startOver(); 
   }
  
  }
  averageValue.push(analogRead(A0));
  delay(5);
  rawmeasurement =  map(averageValue.average(), 0, 1023, -28000, 28000);   //ACS Calibration
  //Serial.println(PackCurrentf/1000);
  //PackCurrentf = (rawmeasurement/1000);

  modbus();
}




