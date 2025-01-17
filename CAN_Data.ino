// *********************** CAN Data ***************************** 
void updateCANData() {
 

// BATTERY MAX CHARGE VOLTAGE: 0.1V, 16 bit UNSIGNED int 
    uint16_t uint16chargevolts = batteryChargeVoltage*10;  // //Target charge voltage (eg 400.0V = 4000 , 16bits long)
    CANData110[0] = uint16chargevolts>>8;
    CANData110[1] = uint16chargevolts;
// BATTERY MAX DISCHARGE VOLTAGE: 0.1V, 16 bit UNSIGNED int 
    uint16_t uint16dischargevolts = batteryDischargeVoltage*10;  // //Target discharge voltage (eg 300.0V = 3000 , 16bits long)
    CANData110[2] = uint16dischargevolts>>8;
    CANData110[3] = uint16dischargevolts;
// BATTERY DISCHARGE LIMIT: 0.1A, 16 bit SIGNED int
    uint16_t uint16dischargeLimit = DischargeCurrentLimit*10;  // //Maximum discharge power allowed (Unit: A+1)
    CANData110[4] = uint16dischargeLimit>>8;       
    CANData110[5] = uint16dischargeLimit; 
// BATTERY CHARGE LIMIT: 0.1A, 16 bit SIGNED int
    uint16_t uint16currentLimit = ChargeCurrentLimit*10;   //Maximum discharge power allowed (Unit: A+1)
    CANData110[6] = uint16currentLimit>>8;       
    CANData110[7] = uint16currentLimit; 
// //SOC (100.00%)
    uint16_t uint16soc2 = SOC*10;  // 
    CANData150[0] = uint16soc2>>8;       
    CANData150[1] = uint16soc2; 
// //SOH (100.00%) //StateOfHealth (100.00%)
    uint16_t uint16stateofhealth = StateOfHealth;  // 
    CANData150[2] = uint16stateofhealth>>8;       
    CANData150[3] = uint16stateofhealth; 
//Remaining capacity (Ah+1) 
    uint16_t uint16remaincapacityf =RemainCapacityf;
    CANData150[4] = uint16remaincapacityf>>8;       
    CANData150[5] = uint16remaincapacityf; 
//Fully charged capacity (Ah+1)
    uint16_t uint16maxcapacity =MaxCapacity;
    CANData150[6] = uint16maxcapacity>>8;       
    CANData150[7] = uint16maxcapacity; 
 //actual Voltage
    uint16_t uint16volts =PackVoltagef*10;
    CANData1D0[0] = uint16volts>>8;       
    CANData1D0[1] = uint16volts; 
 //Current
    uint16_t uint16amps = PackCurrentf*10;
    CANData1D0[2] = uint16amps>>8;       
    CANData1D0[3] = uint16amps; 
  //Temperature average
    uint16_t uint16temp = Temp;
    CANData1D0[4] = uint16temp>>8;       
    CANData1D0[5] = uint16temp; 
  //Temperature max
    CANData210[0] = uint16temp>>8;       
    CANData210[1] = uint16temp; 
  //Temperature min
    CANData210[2] = uint16temp>>8;       
    CANData210[3] = uint16temp; 

    PackCurrentf = (rawmeasurement/1000);

}
