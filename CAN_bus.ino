//  BYD
//
// Send 2s CAN Message : BYD_110
// Send 10s CAN Message : BYD_150 BYD_1D0, BYD_210
// Send 60s CAN message : BYD_190 
// Send initial CAN data once on bootup : BYD_250 BYD_290 BYD_2D0 BYD_3D0_0 BYD_3D0_1 BYD_3D0_2 BYD_3D0_3


void CAN2S() 
{         

  updateCANData();
  //slave.poll( au16data, 16 );

    byte sndStat = CAN0.sendMsgBuf(0x110, 0, 8, CANData110);   // Battery voltage + current limits
    delay(5);
    memset(0x110, 0, 8);
      if(sndStat == CAN_OK){
      Serial.println("Message Sent Successfully!");
      } else {
      Serial.println("Error Sending Message...");
      if(CanError <= 100){
        CanError ++;
       }
      }

} 

void CAN10S() 
{         

  updateCANData();

    CAN0.sendMsgBuf(0x150, 0, 8, CANData150);   // SOC01 //SOH23 //Remaining capacity Ah+1 45// Fully charged capacity AH+1 67
    delay(5);
    memset(0x150, 0, 8);

    CAN0.sendMsgBuf(0x1D0, 0, 8, CANData1D0);   // Voltage (ex 370.0)  01 //Current (ex 81.0A) 23 // Temperature average 45
    delay(5);
    memset(0x1D0, 0, 8);

    CAN0.sendMsgBuf(0x210, 0, 8, CANData210);   //send_cell_info // temp max  01 // temp min 23
    delay(5);
    memset(0x210, 0, 8);

} 

void CAN60S() 
{         

  //updateCANData();

    CAN0.sendMsgBuf(0x190, 0, 8, CANData190);   // alarms
    delay(5);
    //memset(0x190, 0, 8);

} 