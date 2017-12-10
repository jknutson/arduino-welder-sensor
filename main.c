
#include <OneWire.h>
#include <LiquidCrystal.h>
#define sensor A1

// SETUP ONEWIRE SENSOR
OneWire ds(10);  // on pin 10

// SETUP LCD
const int rs = 8, en = 9;
LiquidCrystal lcd(rs, en, 4, 5, 6, 7);

// int analogInput = 0;
float vout = 0.0;
float vin = 0.0;
float R1 = 100000.0; // resistance of R1 (100K) -see text!
float R2 = 10000.0; // resistance of R2 (10K) - see text!

float Vmax = (5*R1 + 5* R2)/R2;

int value = 0;

void readTemp() {
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
  
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  ds.reset_search();
  if ( !ds.search(addr)) {
      Serial.print("No more addresses.\n");
      ds.reset_search();
      return;
  }

  Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }

  if ( addr[0] == 0x10) {
      Serial.print("Device is a DS18S20 family device.\n");
  }
  else if ( addr[0] == 0x28) {
      Serial.print("Device is a DS18B20 family device.\n");
  }
  else {
      Serial.print("Device family is not recognized: 0x");
      Serial.println(addr[0],HEX);
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("P=");
  Serial.print(present,HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print( OneWire::crc8( data, 8), HEX);
  Serial.println();
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;


  if (SignBit) // If its negative
  {
     Serial.print("-");
  }
  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }
  Serial.print(Fract);

  Serial.print("\n");

  
}

void setup(){  
  pinMode(sensor, INPUT);
  Serial.begin(9600);

  Serial.print("Do not input more then (V) : "); // print max input voltage
  Serial.print(Vmax);

  Serial.println(); //skip line
  lcd.begin(16, 2);
  lcd.print("DC VOLTMETER");
}
void loop(){
  //float V = analogRead(sensor); //measuring analog values
  //V = analogRead(sensor) * Vmax / 1024;
  //Serial.println(V); //printing voltage
  
  // read the value at analog input
  value = analogRead(sensor);
  vout = (value * 5.0) / 1024.0; // see text   
  vin = vout / (R2/(R1+R2)); 
  if (vin<0.09) {
    vin=0.0;//statement to quash undesired reading !
  } 
  lcd.setCursor(0, 1);
  lcd.print("INPUT V= ");
  lcd.print(vin);
  delay(500);
}

