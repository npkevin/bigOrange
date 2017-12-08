#include <stdint.h>
#include <math.h>

#include <I2C_16.h>
#include <TMP006.h>

#include <stdint.h>
#include <math.h>
#include <Wire.h>

uint8_t sensor1 = 0x40; // I2C address of TMP006, can be 0x40-0x47
uint16_t samples = TMP006_CFG_1SAMPLE; // # of samples per reading, can be 1/2/4/8/16

#define REV 0
#define FWD 1

#define MOTOR_R 0
#define MOTOR_L 1
#define SPEED 80

#define TURNDIST 1000
#define SENSOR_ACTIVATE_LEVEL 950
#define FRONT_BUMPER 800

#define SENSOR_LINE 800

#define CANDLETEMP 27
#define ultraDistance 12

const byte PWMR = 3;  // PWM control (speed) for motor right
const byte PWML = 11; // PWM control (speed) for motor left
const byte DIRR = 12; // Direction control for motor A
const byte DIRL = 13; // Direction control for motor B

int speedLeft = SPEED;
int speedRight = SPEED;

int foundLine = 0;

int sensorLineR;
int sensorLineL;


struct UltraSonicSensor{
  byte trigPin;
  byte echoPin; 
};

const UltraSonicSensor ultraRight = {10,9};
const UltraSonicSensor ultraLeft = {8,7};
const UltraSonicSensor ultraFrontL = {6,5};
const UltraSonicSensor ultraFrontR = {4,2};

namespace{
long getDistance(const UltraSonicSensor* s){
  long cm, duration;
  // The sensor is triggred by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(s->trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(s->trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(s->trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(s->echoPin, INPUT);
  duration = pulseIn(s->echoPin, HIGH);
 
  // convert the time into a distance
  cm = (duration/2) / 29.1;
  
  return cm;
  
}
}


void setup() {
  // put your setup code here, to run once:
  setupArdumoto(); // Set all pins as outputs
  //getDistance(&ultraRight);
  pinMode(ultraRight.trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(ultraRight.echoPin, INPUT);
  
  pinMode(ultraLeft.trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(ultraLeft.echoPin, INPUT); 
  
  pinMode(ultraFrontL.trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(ultraFrontL.echoPin, INPUT);
  
  pinMode(ultraFrontR.trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(ultraFrontR.echoPin, INPUT); 
  
  //delay(1000);
  delayWBuffer(1000);
}


void loop() {
  
  int sensorEdgeR = analogRead(A1);
  int sensorEdgeL = analogRead(A0);
  
  updateLineSensors();
  
  //printLineSensors(sensorLineL, sensorLineR);
  
  //printEdgeSensors(sensorEdgeL, sensorEdgeR);

  driveFWD(speedRight, speedLeft);
  checkEdges(sensorEdgeR, sensorEdgeL);
 
  //candle code
  //candleFound();
  
  if(!candleFound() && objectInfront()){
    //turn right
    stopALL();
    //delay(500);
    delayWBuffer(500);
    if(objectInfront()){ 
      drive(MOTOR_L,FWD,SPEED*1.1);
      drive(MOTOR_R,REV,SPEED);
      //delay(TURNDIST);
      delayWBuffer(TURNDIST);
    }
    
  }
  printLineSensors(sensorLineL, sensorLineR);
  if(findLine(sensorLineR, sensorLineL) || foundLine){
    //Serial.println(foundLine);
    printLineSensors(sensorLineL, sensorLineR);
    foundLine = 1;
    followLine();//delay of 150 in the function
  }
}



int candleInfront(){
  if(Serial.available() > 0 ) {
     
    char x = Serial.read();
    
    while(!isdigit(x)){
      //Serial.println(x);
      x = Serial.read();
    }
    //Serial.println(x-'0');
    return x - '0';
  }
  return 0;
}

int candleFound(){
   
  if(candleInfront()){
     quickStopAll();
     
     int candleF = 0;
     //keep checking for if candle is infront
     while(candleInfront()){
       Serial.println("candle found");
       candleF = 1;
     }
    
     if(candleF){
       //delay(1000);
       delayWBuffer(1000);
     }
     return 1;
  }
  return 0;
}



int objectInfront(){
  long uFrontR = getDistance(&ultraFrontR);
  long uFrontL = getDistance(&ultraFrontL);
  long uRight = getDistance(&ultraRight);
  long uLeft = getDistance(&ultraLeft);
  
  /*Serial.print(uLeft);
  Serial.print(" ");
  Serial.print(uFrontL);
  Serial.print(" ");
  Serial.print(uFrontR);
  Serial.print(" ");
  Serial.println(uRight);*/
  
  if((uFrontR < ultraDistance && uFrontR > 0)|| (uFrontL < ultraDistance && uFrontL > 0) || (uLeft != 3 && uLeft < ultraDistance && uLeft > 0) || (uRight < ultraDistance && uRight > 0)){
    Serial.println("Object Detected");
    return 1;
  }
  return 0;
  
}

//return 1 if line is found
int findLine(int sensorLineR, int sensorLineL){
  if(sensorLineR > SENSOR_LINE || sensorLineL > SENSOR_LINE){
    return 1;
  }
  return 0;
}

//try to follow the line
void followLine(){
  
  // on the line
  /*if(sensorLineR > SENSOR_LINE || sensorLineL > SENSOR_LINE){ 
     driveFWD(speedRight, speedLeft);
     delay(150);
  }*/
  //update sensor
  updateLineSensors();
  
  //printLineSensors(sensorLineL, sensorLineR);
  Serial.println(sensorLineR);
  //if it is completly off the line
  if(sensorLineR < SENSOR_LINE && sensorLineL < SENSOR_LINE){
    //turn left and right to find line
    long timer = millis();
    while(!candleFound() && millis() - timer < 500 && sensorLineL < SENSOR_LINE && sensorLineR < SENSOR_LINE){
     //turn left
      //printLineSensors(sensorLineL, sensorLineR);
      drive(MOTOR_R,FWD,SPEED);
      drive(MOTOR_L,REV,SPEED);
      //delay(100);
      updateLineSensors();
    }
    //stopALL();
    //delay(300);
    //updateLineSensors();
    //if sensor still havent found the line
    if(sensorLineL < SENSOR_LINE && sensorLineR < SENSOR_LINE){
      timer = millis();
      while(!candleFound() && millis() - timer < 700*2 && sensorLineL < SENSOR_LINE && sensorLineR < SENSOR_LINE){//7
        //turn right
        drive(MOTOR_R,REV,SPEED);
        drive(MOTOR_L,FWD,SPEED);
        updateLineSensors();
      }
      stopALL();
      if(sensorLineL < SENSOR_LINE && sensorLineR < SENSOR_LINE){
        Serial.println("didn't find line");
        foundLine = 0;
      }else{
         Serial.println("found line again!!!");
      }
    }else{
      Serial.println("found line again!!!");
    }
    
  //both sensors on the black line -> go straight
  }else if(sensorLineR > SENSOR_LINE && sensorLineL > SENSOR_LINE){
    speedRight = SPEED;
    speedLeft = SPEED;
  }else{
      Serial.print("SensorR: ");
      Serial.print(sensorLineR);
      Serial.print("     ");
      Serial.print("SensorL: ");
      Serial.println(sensorLineL);
    if(sensorLineR-sensorLineL > 70){
      Serial.print("turning right slightly ");
      
      //stopALL();
      //delay(200);
      long timer = millis();
      while(millis() - timer < 500 && sensorLineL < SENSOR_LINE){ //&& !candleFound()){
        //turn right
        drive(MOTOR_R,REV,SPEED);
        drive(MOTOR_L,FWD,SPEED*1.1);
        updateLineSensors();
      }
      
      //turn right slightly
      //speedRight = speedRight*0.9;
      //speedLeft = SPEED;
    }else if(sensorLineL-sensorLineR > 70){
      Serial.print("turning left slightly ");
      //stopALL();
      //delay(200);
      //turn left slightly
      long timer = millis();
      while(millis() - timer < 500 && sensorLineR < SENSOR_LINE){//&& !candleFound()){
        //turn left
        drive(MOTOR_R,FWD,SPEED);
        drive(MOTOR_L,REV,SPEED*1.1);
        updateLineSensors();
      }
    }
  }
}


void checkEdges(int sensorEdgeR, int sensorEdgeL){
  //both edge sensors activated
  /*if(checkSensor(sensorEdgeR) && checkSensor(sensorEdgeL))
  {
    driveREV(SPEED);
    delay(200);
  //right edge sensor activated
  } else*/ if (checkSensor(sensorEdgeL)){
    //turn right
    stopALL();
    drive(MOTOR_L,FWD,SPEED*1.1);
    drive(MOTOR_R,REV,SPEED);
    //delay(TURNDIST);
    delayWBuffer(TURNDIST);
    
  } else if (checkSensor(sensorEdgeR)){ //left edge sensor activated
   //turn left
   stopALL();
   drive(MOTOR_R,FWD,SPEED);
   drive(MOTOR_L,REV,SPEED*1.1);
   //delay(TURNDIST);
   delayWBuffer(TURNDIST);
  }
  
}

void delayWBuffer(long time){
  long m = millis();
  while(millis()-m < time){
    Serial.read();
  }
  
}


int checkSensor(int sensor)
{
  if(sensor > SENSOR_ACTIVATE_LEVEL)
  {
    return 1;
  }

  return 0;
}

void updateLineSensors(){
  sensorLineR = analogRead(A2);
  sensorLineL = analogRead(A3);
}

void driveFWD(int right, int left)
{
  drive(MOTOR_R,FWD,right);
  drive(MOTOR_L,FWD,left+8);//10% of 80 more was found to be straight
}

void driveREV(int speed)
{
  drive(MOTOR_R,REV,speed);
  drive(MOTOR_L,REV,speed);
}

void printEdgeSensors(int left, int right)
{
  Serial.print("left: ");
  Serial.print(left);
  Serial.println();
  Serial.print("right: ");
  Serial.print(right);
  Serial.println("\n"); 
}

void printLineSensors(int left, int right)
{
  Serial.print("left: ");
  Serial.print(left);
  Serial.println();
  Serial.print("right: ");
  Serial.print(right);
  Serial.println("\n");
}

void drive (byte motor, byte dir, byte spd)
{
  if (motor == MOTOR_R)
  {
    digitalWrite(DIRR, dir);
    analogWrite(PWMR, spd);
  }
  else if (motor == MOTOR_L)
  {
    digitalWrite(DIRL, dir);
    analogWrite(PWML, spd);
  }  
}

void quickStopAll(){
  driveREV(SPEED);
  delay(30);
  stopALL();
}

void stopALL()
{
  stop(MOTOR_R);
  stop(MOTOR_L);
}

void stop(byte motor)
{
  drive(motor, 0, 0);
}

void setupArdumoto()
{
  // All pins should be setup as outputs:
  Serial.begin(9600);
  pinMode(PWMR, OUTPUT);
  pinMode(PWML, OUTPUT);
  pinMode(DIRR, OUTPUT);
  pinMode(DIRL, OUTPUT);

  // Initialize all pins as low:
  digitalWrite(PWMR, LOW);
  digitalWrite(PWML, LOW);
  digitalWrite(DIRR, LOW);
  digitalWrite(DIRL, LOW);
}
