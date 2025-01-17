void modbus() 
{
  slave.poll( au16data, 16 );

  PackVoltagef = au16data[0];            //modbus register 0 = battery voltage
  SOC = au16data[1];                     //modbus register 1 = SOC
  Temp = au16data[2];            //modbus register 2 = battery temperature
  ChargeCurrentLimit =  au16data[3];     //modbus register 3 = charge current limit
  DischargeCurrentLimit = au16data[4];   //modbus register 4 = discharge current limit
  RemainCapacityf = au16data[5];         //modbus register 5 = remaining capacity X10kWh
  au16data[6] = INITIALMSG;              //modbus register 6 = Initial message received
  au16data[7] = CanError;                //modbus register 7 = CAN error counter
  au16data[8] = rawmeasurement;          //modbus register 8 = current in MilliAmps
}