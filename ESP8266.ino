/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//Deklerasi Pin Relay
#define R1 D0 //Master Relay Inverter
#define R2 D1 //Sirine
#define R3 D2 //Pump
#define R4 D3 //Pump Tank
#define R5 D4 //Lamp
#define R6 D5 //Fan
#define R7 D6
#define R8 D7

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPLAKB7G8tV"
#define BLYNK_TEMPLATE_NAME         "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "69la8rmhopz93bcCfXTg27HJISiFCU3X"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "p";
char pass[] = "Kata Sandi";

//Deklarasi untuk parsing data
String dataIn;
String dt[25];
int i;
boolean parsing=false;

//Definisi data string
String ultrasonic = "";
String Voltppm = "";
String Resistanceppm = "";
String ppm = "";
String lembabTanah = "";
String Voltage = "";
String Temp = "";
String Humidity = "";
String PIR = "";
String indexppm = "";
String statPIR = "";
String tair = "";
String Tegangan = "";
String Arus = "";
String Daya = "";
String LDR = "";
String MQ6 = "";
String statMQ6 = "";
String statLDR = "";
String voltAC = "";
String ampAC = "";
String powerAC = "";
String energyAC = "";
String freqAC = "";
String pfAC = "";

//String Switch For Master Relay
String Inverter = ""; //Indicator R1 (Master relay inverter)
String Sirine = ""; //R2 Warning For GAS (MQ6)
String Pump = ""; //R3 Plant Pump
String Sanyo = ""; //R4 Pompa AC (Ultrasonic)
String Lamp = ""; //R5 Lampu (LDR)
String Fan = ""; //R6 Kipas (PIR)

BlynkTimer timer;

void sendSensor()
{
  float ppmF = ppm.toFloat(); 

   if(ppmF > 0 && ppmF <= 50){
    indexppm = "Baik";
  } else if(ppmF > 51 && ppmF <=100){
    indexppm = "Sedang";
  } else if(ppmF > 101 && ppmF <=199){
    indexppm = "Buruk";
  } else if(ppmF > 200 && ppmF <=299){
    indexppm = "Buruk+";
  } else if(ppmF > 300 && ppmF <=500){
    indexppm = "Danger";
  } else if(ppmF > 500){
    indexppm = "Danger+";
  } else {
    indexppm = "error";
  }

  if(PIR == "1"){
    statPIR = "ON";
  } else { 
    statPIR = "OFF";
  }

  if(LDR == "1"){
    statLDR = "ON";
  } else {
    statLDR = "OFF";
  }

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V1, Humidity+" %");
  Blynk.virtualWrite(V2, Temp+" °C");
  Blynk.virtualWrite(V3, ultrasonic+ "cm"); 
  Blynk.virtualWrite(V0, ppm+" ppm"); 
  Blynk.virtualWrite(V4, indexppm); 
  Blynk.virtualWrite(V5, Voltage+" Volt");  
  Blynk.virtualWrite(V6, lembabTanah+" %");  
  Blynk.virtualWrite(V7, statPIR);  
  Blynk.virtualWrite(V8, tair+" %");  
  Blynk.virtualWrite(V9, Tegangan+" Volt");  
  Blynk.virtualWrite(V10, Arus+" A");  
  Blynk.virtualWrite(V11, Daya+" Watt"); 
  Blynk.virtualWrite(V12, statLDR); 
  Blynk.virtualWrite(V13, statMQ6); 
  Blynk.virtualWrite(V14, voltAC+" V"); 
  Blynk.virtualWrite(V15, ampAC+" A"); 
  Blynk.virtualWrite(V16, powerAC+" Watt"); 
  Blynk.virtualWrite(V17, energyAC+" KWh"); 
  Blynk.virtualWrite(V18, freqAC+" Hz");   
  Blynk.virtualWrite(V19, pfAC);
  Blynk.virtualWrite(V20, Inverter);
  Blynk.virtualWrite(V21, Sirine);
  Blynk.virtualWrite(V22, Pump);
  Blynk.virtualWrite(V23, Sanyo);
}

void setup() {
   Serial.begin(9600);
   dataIn="";

   //Pinmode relay
    pinMode(R1, OUTPUT);
    pinMode(R2, OUTPUT);
    pinMode(R3, OUTPUT);
    pinMode(R4, OUTPUT);
    pinMode(R5, OUTPUT);
    pinMode(R6, OUTPUT);
    pinMode(R7, OUTPUT);
    pinMode(R8, OUTPUT);

    //Set OFF saat starting 
    digitalWrite(R1, HIGH);
    digitalWrite(R2, HIGH);
    digitalWrite(R3, HIGH);
    digitalWrite(R4, HIGH);
    digitalWrite(R5, HIGH);
    digitalWrite(R6, HIGH);
    digitalWrite(R7, HIGH);
    digitalWrite(R8, HIGH);
    //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(3000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
    if(Serial.available()>0) {
    char inChar = (char)Serial.read();
    dataIn += inChar;
    if (inChar == '\n') {
    parsing = true;
  }
}
 
if(parsing){
    parsingData();
    parsing=false;
    dataIn="";
  }
}

void parsingData(){
int j=0;
 
//kirim data yang telah diterima sebelumnya
Serial.print("data masuk : ");
Serial.print(dataIn);
 
//inisialisasi variabel, (reset isi variabel)
dt[j]="";
//proses parsing data
for(i=1;i<dataIn.length();i++){
//pengecekan tiap karakter dengan karakter (#) dan (,)
if ((dataIn[i] == '#') || (dataIn[i] == ','))
{
//increment variabel j, digunakan untuk merubah index array penampung
j++;
dt[j]="";       //inisialisasi variabel array dt[j]
}
else
{
//proses tampung data saat pengecekan karakter selesai.
dt[j] = dt[j] + dataIn[i];
}
}
 
//kirim data hasil parsing
Serial.print("data 1 (distance) : ");
Serial.println(dt[0]);
Serial.print("data 2 (VRL) : ");
Serial.println(dt[1]);
Serial.print("data 3 (Rs) : ");
Serial.println(dt[2]);
Serial.print("data 4 (ppm) : ");
Serial.println(dt[3]);
Serial.print("data 5 (Tanah) : ");
Serial.println(dt[4]);
Serial.print("data 6 (Solar) : ");
Serial.println(dt[5]);
Serial.print("data 7 (Temp) : ");
Serial.println(dt[6]);
Serial.print("data 8 (Humidity) : ");
Serial.println(dt[7]);
Serial.print("data 9 (PIR) : ");
Serial.println(dt[8]);
Serial.print("data 10 (Water Tank) : ");
Serial.println(dt[9]);
Serial.print("data 11 (Battery) : ");
Serial.println(dt[10]);
Serial.print("data 12 (Arus DC) : ");
Serial.println(dt[11]);
Serial.print("data 13 (Daya DC) : ");
Serial.println(dt[12]);
Serial.print("data 14 (LDR) : ");
Serial.println(dt[13]);
Serial.print("data 15 (MQ6) : ");
Serial.println(dt[14]);
Serial.print("data 16 (Volt AC) : ");
Serial.println(dt[15]);
Serial.print("data 17 (Current AC) : ");
Serial.println(dt[16]);
Serial.print("data 18 (Power AC) : ");
Serial.println(dt[17]);
Serial.print("data 19 (Energy AC) : ");
Serial.println(dt[18]);
Serial.print("data 20 (Frequency AC) : ");
Serial.println(dt[19]);
Serial.print("data 21 (Power Factor) : ");
Serial.println(dt[20]);
Serial.print("\n\n");

//Wrapping data untuk di definisikan
ultrasonic = dt[0];
Voltppm = dt[1];
Resistanceppm = dt[2];
ppm = dt[3];
lembabTanah = dt[4];
Voltage = dt[5];
Temp = dt[6];
Humidity = dt[7];
PIR = dt[8];
tair = dt[9];
Tegangan = dt[10];
Arus = dt[11];
Daya = dt[12];
LDR = dt[13];
MQ6 = dt[14];
voltAC = dt[15];
ampAC = dt[16];
powerAC = dt[17];
energyAC = dt[18];
freqAC = dt[19];
pfAC = dt[20];

//Relay Controll
//hidupkan relay saat kondisi sensor membaca jarak 140 cm
if (ultrasonic.toInt() >= 140){
  digitalWrite(R4,LOW);
  Sanyo = "ON";
}
//matikan relay saat kondisi sensor membaca jarak 10 cm
if (ultrasonic.toInt() <= 10){
  digitalWrite(R4,HIGH);
  Sanyo = "OFF";
}

if (PIR == "1"){
  digitalWrite(R6, LOW);
  Fan = "ON";
} else {
  digitalWrite(R6, HIGH);
  Fan = "OFF";
}

if(lembabTanah.toInt() >= 5){
  digitalWrite(R3, LOW);
  Pump = "ON";
} 

if(lembabTanah.toInt() <= 60){
  digitalWrite(R3, HIGH);
  Pump = "OFF";
} 

if(LDR == "1"){
  digitalWrite(R5, LOW);
  Lamp = "ON";
} else {
  digitalWrite(R5, HIGH);
  Lamp = "OFF";
}

if(MQ6 == "1"){
  digitalWrite(R2, LOW);
  statMQ6 = "Gas Bocor";
  Sirine = "ON";
} else {
  digitalWrite(R2, HIGH);
  statMQ6 = "Gas Aman";
  Sirine = "OFF";
}

if(Sanyo == "ON" || Fan == "ON" || Lamp == "ON"){
  digitalWrite(R1, LOW);
  Inverter = "ON";
} else {
  digitalWrite(R1, HIGH);
  Inverter = "OFF";
}

//------------------Ga usah Delay--------------------//
}
