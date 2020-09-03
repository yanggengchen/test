#include "MsTimer2.h"
#include "SAF.h"
#include <Wire.h>
#include <SoftwareSerial.h>
#define SLAVE_ADDRESS 0x04

struct Package {
  double weight[3];
  double alchohol;
  double ammonia;
};

typedef struct Package_gas{
  unsigned char head;
  unsigned char reserved_head[3];
  unsigned long alcohol_left;
  unsigned long alcohol_right;
  unsigned long ammonia_left;
  unsigned long ammonia_right;
  unsigned char reserved_tail[2];
  unsigned char valid;
  unsigned char tail;
}Package_gas;

typedef struct Package_weight_L{
  unsigned char head;
  unsigned char reserved_head[3];
  unsigned long weight1_left;
  unsigned long weight1_right;
  unsigned long weight2_left;
  unsigned long weight2_right;
  unsigned char reserved_tail[2];
  unsigned char valid;
  unsigned char tail;
}Package_weight_L;

typedef struct Package_weight_R{
  unsigned char head;
  unsigned char reserved_head[3];
  unsigned long weight1_left;
  unsigned long weight1_right;
  unsigned long weight2_left;
  unsigned long weight2_right;
  unsigned char reserved_tail[2];
  unsigned char valid;
  unsigned char tail;
}Package_weight_R;

SoftwareSerial softSerial(2, 4);

const int SAMPLE_RATE = 10;
int data = 0;
String msg = "";
Package_gas package_gas;
Package_weight_L package_weight_L;
Package_weight_R package_weight_R;
Package pack;

const int PIN_ALCHOHOL = A0;
Signal<SAMPLE_RATE * 10> alchohol;
const int PIN_NH3 = A1;
Signal<SAMPLE_RATE * 10> ammonia;
const int PIN_WEIGHT_FIRST = A2;
Signal<SAMPLE_RATE * 10> weight[0];
const int PIN_WEIGHT_SECOND = A3;
Signal<SAMPLE_RATE * 10> weight[1];
const int PIN_WEIGHT_THIRD = A4;
Signal<SAMPLE_RATE * 10> weight[2];
const int PIN_WEIGHT_FOURTH = A5;
Signal<SAMPLE_RATE * 10> weight[3];

void
ISRMsTimer2() {
  DSPUpdate(alchohol, (unsigned short)analogRead(PIN_ALCHOHOL));
  DSPUpdate(ammonia, (unsigned short)analogRead(PIN_NH3));
  DSPUpdate(weight[0], (unsigned short)analogRead(PIN_WEIGHT_FIRST));
  DSPUpdate(weight[1], (unsigned short)analogRead(PIN_WEIGHT_SECOND));
  DSPUpdate(weight[2], (unsigned short)analogRead(PIN_WEIGHT_THIRD));
  DSPUpdate(weight[3], (unsigned short)analogRead(PIN_WEIGHT_FOURTH));
}

void
setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  softSerial.begin(9600);
  delay(10);
  
  DSPinit(alchohol);
  DSPinit(ammonia);
  DSPinit(weight[0]);
  DSPinit(weight[1]);
  DSPinit(weight[2]);
  DSPinit(weight[3]);
  
  MsTimer2::set(1000 / SAMPLE_RATE, ISRMsTimer2);
  MsTimer2::start();

  Wire.begin(SLAVE_ADDRESS);
  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData); 
  Serial.println("Ready");
}

void
loop() {
   delay(100);
}

// callback for received data
void receiveData(int byteCount){
  while(Wire.available()) {
    data = Wire.read();
    Serial.print("data received: ");
    Serial.println(data);

    pack.alchohol = SAFAverage(alchohol).current;
    pack.ammonia = SAFAverage(ammonia).current;
    pack.weight[0] = SAFAverage(weight[0]).current;
    pack.weight[1] = SAFAverage(weight[1]).current;
    pack.weight[2] = SAFAverage(weight[2]).current;
    pack.weight[3] = SAFAverage(weight[3]).current;
    
//Gas Package
    package_gas.valid = 0;
    package_gas.head = 0x01;
    package_gas.tail = 0xff;
    package_gas.alcohol_left = (unsigned long)(pack.alcohol);
    package_gas.alcohol_right = (unsigned long)((alcohol - (unsigned long)alcohol) * 65536); 
    package_gas.ammonia_left = (unsigned long)(ammonia);
    package_gas.ammonia_right = (unsigned long)((ammonia - (unsigned long)ammonia) * 65536);
    

//Left Weight Package
    package_weight_L.valid = 0;
    package_weight_L.head = 0x02;
    package_weight_L.tail = 0xff;
    const double weight1 = 213.12837123;
    const double weight2 = 12376.2938712;
    package_weight_L.weight1_left = (unsigned long)(weight1);
    package_weight_L.weight1_right = (unsigned long)((weight1 - (unsigned long)weight1) * 65536); 
    package_weight_L.weight2_left = (unsigned long)(weight2);
    package_weight_L.weight2_right = (unsigned long)((weight2 - (unsigned long)weight2) * 65536);


//Right Weight Package
    package_weight_R.valid = 0;
    package_weight_R.head = 0x03;
    package_weight_R.tail = 0xff;
    const double weight3 = 213.12837123;
    const double weight4 = 12376.2938712;
    package_weight_R.weight1_left = (unsigned long)(weight3);
    package_weight_R.weight1_right = (unsigned long)((weight3 - (unsigned long)weight3) * 65536); 
    package_weight_R.weight2_left = (unsigned long)(weight4);
    package_weight_R.weight2_right = (unsigned long)((weight4 - (unsigned long)weight4) * 65536);
  }
}

// callback for sending data
void sendData(){
  if (data == 1){
    Wire.write((char *)(&package_gas), sizeof(Package_gas));
  }
  else if (data == 2){
    Wire.write((char *)(&package_weight_L), sizeof(Package_weight_L));
  }
  else{
    Wire.write((char *)(&package_weight_R), sizeof(Package_weight_R));
  }
  
}
