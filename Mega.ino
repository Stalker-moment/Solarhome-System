#include "Wire.h" // For I2C
#include "LCD.h" // For LCD
#include "LiquidCrystal_I2C.h" // Added library*
#include <Adafruit_INA219.h>
#include "DHT.h"
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

//PZEM004Tv30 pzem(14, 15); // Software Serial pin 11 (RX) & 12 (TX)
PZEM004Tv30 pzem(&Serial3);  // Menggunakan Hardware Serial 3 TX RX ke 3 di balik

//Set the pins on the I2C chip used for LCD connections
//ADDR,EN,R/W,RS,D4,D5,D6,D7
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the default I2C bus address of the backpack-see article
const int INA_addr = 0x40;  // INA219 address
Adafruit_INA219 ina219(INA_addr);

//IO sensor Ultrasonic
#define echoPin 24 //Echo Pin
#define trigPin 26 //Trigger Pin
#define SoilSensor A1 //Soil Sensor
#define mq7sensor A0 // MQ7 Sensor
#define voltagesensor A2 //voltage sensor panel
#define DHTPIN 28
#define DHTTYPE DHT22
#define PIR 30
#define voltagebat A4  //voltage sensor battery
#define LDR 32 //LDR LAMPU
#define MQ6 A5

double tegangan = 00;
double nilaiarus = 00;
float daya;

int statusPin = 0;

DHT dht(DHTPIN, DHTTYPE);

String piring = "";
String indexppm;
String tempe;
String humidy;
String vbat;
String tair;
String ppmvalue;
String motionindex;
String soilvalue;
String stegangan;
String sarus;
String sdaya;
String vbatuy;
String LDRout;
String alertmq6;
String voltAC;
String ampAC;
String powerAC;
String energyAC;
String freqAC;
String pfAC;

//FLOAT FOR PZEM
float voltage,current,power,energy,frequency,pf;

int maximumRange = 140; //kebutuhan akan maksimal range
int minimumRange = 00; //kebutuhan akan minimal range
long duration, distance; //waktu untuk kalkulasi jarak

long RL = 1000; // 1000 Ohm
long Ro = 830; // 830 ohm ( SILAHKAN DISESUAIKAN)

// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;

// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 
 
// Float for Reference Voltage
float ref_voltage = 5.0;
 
// Integer for ADC value
int adc_value = 0;

// Floats for ADC voltage & Input voltage
float adc_voltage2 = 0.0;
float in_voltage2 = 0.0;
 
// Floats for resistor values in divider (in ohms)
float R12 = 30000.0;
float R22 = 7500.0; 
 
// Float for Reference Voltage
float ref_voltage2 = 5.0;
 
// Integer for ADC value
int adc_value2 = 0;

//MQ 7 variable
float nilaimq6;
float teganganmq6;

void setup() {
Serial.begin (9600); //inisialiasasi komunikasi serial

// Set on LCD module
lcd.begin (20,4); // 16 x 2 LCD module
lcd.setBacklightPin(3,POSITIVE); // BL, BL_POL
lcd.setBacklight(HIGH);
lcd.clear();
lcd.noCursor();

//-----------------------------< deklarasi pin >-----------------------------//
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
pinMode(PIR, INPUT);
pinMode(LDR, INPUT);

//-----------------------------< Start DHT >----------------------------------//
dht.begin();

//-----------------------------< Start INA219 >-------------------------------//
ina219.begin();

//-----------------------------< WELCOME TEXT >--------------------------------//
lcd.setCursor(6, 1);
lcd.print("WELCOME");
lcd.setCursor(6, 2);
lcd.print("Brother");
delay(2000);
lcd.clear();
lcd.setCursor(0, 1);
lcd.print("  SMARTHOME SYSTEM  ");
lcd.setCursor(1, 2);
lcd.print(" With IOT Control  ");
lcd.setCursor(0, 3);
delay(2000);
lcd.clear();
lcd.setCursor(2, 0);
lcd.print(">>> Made By <<<");
lcd.setCursor(6, 1);
lcd.print("Robotics");
lcd.setCursor(6, 2);
lcd.print("SMK SMTI");
lcd.setCursor(5, 3);
lcd.print("YOGYAKARTA");
delay(2000);
lcd.clear();
lcd.setCursor(3, 1);
lcd.print("System Start");
lcd.setCursor(7, 2);
lcd.print("in 3");
delay(1000);
lcd.clear();
lcd.setCursor(3, 1);
lcd.print("System Start");
lcd.setCursor(7, 2);
lcd.print("in 2");
delay(1000);
lcd.clear();
lcd.setCursor(3, 1);
lcd.print("System Start");
lcd.setCursor(7, 2);
lcd.print("in 1");;
delay(1000);
lcd.clear();
}
 
void loop() {
//-------------------------------< LDR READ >------------------------------------//
int LDRdata = digitalRead(LDR);

//-------------------------------< SOIL READ >----------------------------------//
float kelembabanTanah;
int hasilPembacaan = analogRead(SoilSensor);
kelembabanTanah = (100 - ((hasilPembacaan/1023.00)*100));

//------------------------------< Read DHT22 >-----------------------------------//
float temp = dht.readTemperature();
float humidity = dht.readHumidity();

//-----------------------< DHT REMOVE NAN DISPLAY >---------------------------//
if(isnan(temp)){
  tempe = "-";
} else{
  tempe = temp;
}

if(isnan(humidity)){
  humidy = "-";
} else {
  humidy = humidity;
}

//--------------------< LCD DISPLAY 2 >---------------------//
//Serial.print("Persentase Kelembaban Tanah = ");
//Serial.print(kelembabanTanah);
//Serial.println("%");
lcd.print("Udara: "+ppmvalue+" ppm");
lcd.setCursor(0, 1);
lcd.print("Air Index: "+indexppm);  
lcd.setCursor(0,2);
lcd.print("Temp: "+tempe+" degC");  
lcd.setCursor(0, 3);
lcd.print("Humidity: "+humidy+" %");
delay(1000);

//-----------MQ 6--------------//
nilaimq6 = analogRead(MQ6);
teganganmq6 = (nilaimq6/1023)*5;

//------------< Alert LPG >----------------//
if(teganganmq6 > 1){
  alertmq6 = "1";
} else {
  alertmq6 = "0";
}

//-----------MQ 7--------------//
int sensorvalue = analogRead(mq7sensor); // membaca nilai ADC dari sensor
float VRL= sensorvalue*5.00/1024;  // mengubah nilai ADC ( 0 - 1023 ) menjadi nilai voltase ( 0 - 5.00 volt )
//Serial.print("VRL : ");
//Serial.print(VRL);
//Serial.println(" volt");

float Rs = ( 5.00 * RL / VRL ) - RL;
//Serial.print("Rs : ");
//Serial.print(Rs);
//Serial.println(" Ohm");

float ppm = 100 * pow(Rs / Ro,-1.53); // ppm = 100 * ((rs/ro)^-1.53);
//Serial.print("CO : ");
//Serial.print(ppm);
//Serial.println(" ppm");

//Serial.println();
delay(500);

//------------------------------< LCD DISPLAY 3 >--------------------------------//
lcd.clear();
lcd.print("Water Tank: "+tair+" %");
lcd.setCursor(0, 1);
lcd.print("Distance: "+String(distance)+ " Cm");
lcd.setCursor(0, 2);
lcd.print("Tanah: "+soilvalue+ " %");
lcd.setCursor(0, 3);
lcd.print("Lamp: "+motionindex);

//------------------------------< Ultrasonic >-----------------------------------//
digitalWrite(trigPin, LOW);delayMicroseconds(2);
digitalWrite(trigPin, HIGH);delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
//-------------------< Short delay for ultrasonic >---------------------//
delay(500);
 
//perhitungan untuk dijadikan jarak
distance = duration/58.2;

//---------------------------< Solar Voltage >---------------------------------//
adc_value = analogRead(voltagesensor);
   
// Determine voltage at ADC input
adc_voltage  = (adc_value * ref_voltage) / 1024.0; 

// Calculate voltage at divider input
in_voltage = adc_voltage / (R2/(R1+R2)) ; 

//----------------------< Battery Singgle Voltage >---------------------------//
adc_value2 = analogRead(voltagebat);

// Determine voltage at ADC input
adc_voltage2  = (adc_value2 * ref_voltage2) / 1024.0; 

// Calculate voltage at divider input
in_voltage2 = adc_voltage2 / (R2/(R1+R2));
// Print results to Serial Monitor to 2 decimal places
//Serial.print("Input Voltage = ");
//Serial.println(in_voltage, 2);

//---------------------------< PIR detect >-----------------------------//
statusPin = digitalRead(PIR);
if(statusPin == HIGH){
  piring = "1";
  motionindex = "ON";
}else{
  piring = "0";
  motionindex = "OFF";
}

//----------------------------< Index MQ 7 >----------------------------//
if(ppm > 0 && ppm <= 50){
  indexppm = "Baik";
} else if(ppm > 51 && ppm <=100){
  indexppm = "Sedang";
} else if(ppm > 101 && ppm <=199){
  indexppm = "Buruk";
} else if(ppm > 200 && ppm <=299){
  indexppm = "Buruk+";
} else if(ppm > 300 && ppm <=500){
  indexppm = "Danger";
} else if(ppm > 500){
  indexppm = "Danger+";
} else {
  indexppm = "error";
}
//-------------------[ Convert Float,Int,Double to String ]-------------------//
vbat = String(in_voltage, 2);
//vbatuy = String(in_voltage2, 2);
ppmvalue = ppm;
soilvalue = kelembabanTanah;
LDRout = LDRdata;

//---------------------< INA219 READ (No include Inverter) >----------------------//
float busVoltage = 0;
float currentDC = 0; // Measure in milli amps
float powerDC = 0;

busVoltage = ina219.getBusVoltage_V();
currentDC = ina219.getCurrent_mA();
powerDC = busVoltage * (currentDC/1000); // Calculate the Power
sarus = String(currentDC);
sdaya = powerDC; 
vbatuy = String(busVoltage);

delay(1000);

/*//------------------< Read Current Sensor >-------------------------//
float vbatu = float(in_voltage2, 2);
daya = vbatu * nilaiarus; //rumus untuk mendapatkan nilai watt
stegangan = String(vbatu);
sarus = String(nilaiarus);
sdaya = String(daya); 
*/

//--------------------< LCD DISPLAY 4 >--------------------------//
lcd.clear();
lcd.print("Solar: "+vbat+ " V");
lcd.setCursor(0, 1);
lcd.print("Battery: "+vbatuy+ " V");
lcd.setCursor(0, 2);
lcd.print("Current Dc: "+sarus+ "mA");
lcd.setCursor(0, 3);
lcd.print("Watt Dc: "+sdaya+ " W");

//------------------------------< PZEM 004T 100A Measurement >-------------------------------//
    voltage = pzem.voltage();
     if(isnan(voltage)){
          voltAC = "-";
         //Serial.print("Voltage: "); 
         //Serial.print(voltage);
         //Serial.println("V");
     } else {
         voltAC = voltage;
         //Serial.println("Error reading voltage");
     }

     current = pzem.current();
     if(isnan(current)){
         ampAC = "-";
         //Serial.print("Current: ");
         //Serial.print(current);
         //Serial.println("A");
     } else {
         ampAC = current;
         //Serial.println("Error reading current");
     }

     power = pzem.power();
     if(isnan(power)){
         powerAC = "-";
         //Serial.print("Power: ");
         //Serial.print(power);
         //Serial.println("W");
     } else {
         powerAC = power;
         //Serial.println("Error reading power");
     }

     energy = pzem.energy();
     if(isnan(energy)){
         energyAC = "-";
         //Serial.print("Energy: ");
         //Serial.print(energy,3);
         //Serial.println("kWh");
     } else {
         energyAC = energy;
         //Serial.println("Error reading energy");
     }

     frequency = pzem.frequency();
     if(isnan(frequency)){
         freqAC = "-";
         //Serial.print("Frequency: ");
         //Serial.print(frequency, 1);
         //Serial.println("Hz");
     } else {
         freqAC = frequency;
         //Serial.println("Error reading frequency");
     }

     pf = pzem.pf();
     if(isnan(pf)){
         pfAC = "-";
         //Serial.print("PF: ");
         //Serial.println(pf);
     } else {
         pfAC = pf;
         //Serial.println("Error reading power factor");
     }

//---------------------------< persentase tanki >-----------------------------//
tair = int(map(distance, 5, 140, 100, 0));

//---------------------[ Send Serial To Nodemcu ESP8266 ]----------------------//
Serial.println("*"+String(distance)+","+String(VRL)+","+String(Rs)+","+String(ppm)+","+String(kelembabanTanah)+","+String(in_voltage, 2)+","+String(tempe)+","+String(humidy)+","+piring+","+tair+","+vbatuy+","+sarus+","+sdaya+","+LDRout+","+alertmq6+","+voltAC+","+ampAC+","+powerAC+","+energyAC+","+freqAC+","+pfAC+"#"); //inject wrapping
//Serial.println("U = "+String(distance));Serial.println("V = "+String(VRL));Serial.println("R ="+String(Rs));Serial.println("C ="+String(ppm));Serial.println("T ="+String(kelembabanTanah));Serial.println("VL ="+String(in_voltage, 2));

//------------------< DISPLAY LCD 5 & 6 Measure AC OUTPUT >--------------------//
delay(1500);
lcd.clear();
lcd.print("--< INVERTER OUT >--");
lcd.setCursor(0, 1);
lcd.print("Voltage: "+voltAC+ " V");
lcd.setCursor(0, 2);
lcd.print("Current: "+ampAC+ " A");
lcd.setCursor(0, 3);
lcd.print("Power: "+powerAC+ " W");
delay(1500);
lcd.clear();
lcd.print("--< INVERTER OUT >--");
lcd.setCursor(0, 1);
lcd.print("Energy: "+energyAC+ " KWh");
lcd.setCursor(0, 2);
lcd.print("Frequency: "+freqAC+ " Hz");
lcd.setCursor(0, 3);
lcd.print("PF "+pfAC);
delay(1500);
lcd.clear();
}
