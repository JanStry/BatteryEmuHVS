void modbus() 
{
  slave.poll( au16data, 16 );

  PackVoltagef = au16data[0];            //modbus register 0 = battery voltage
  SOC = au16data[1];                     //modbus register 1 = SOC
  Temp = au16data[2];                    //modbus register 2 = battery temperature
  ChargeCurrentLimit =  au16data[3];     //modbus register 3 = charge current limit
  DischargeCurrentLimit = au16data[4];   //modbus register 4 = discharge current limit
  RemainCapacityf = au16data[5];         //modbus register 5 = remaining capacity X10kWh
  StateOfHealth = au16data[6];           //modbus register 6 = SOH
  au16data[7] = MODUS;                   //modbus register 7 = Emulator modus
  au16data[8] = CanError;                //modbus register 8 = CAN error counter
  au16data[9] = rawmeasurement;          //modbus register 9 = current in MilliAmps
  RELAY = au16data[10];                  //modbus register 10 = RELAY control
  MODESET = au16data[11];                //modbus register 11 = SET Emulator modus
}