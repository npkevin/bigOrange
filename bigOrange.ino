
#define REV 0
#define FWD 1

#define MOTOR_R 0
#define MOTOR_L 1
#define SPEED 80//100isgood
#define OFFSET 0
#define SENSOR_ACTIVATE_LEVEL 950
#define SENSOR_ACTIVATE_LEVEL_F 850
#define FRONT_BUMPER 800
#define blockDistance 750


#define wiggleSPEED 70 //70
#define wiggleTURN 250
#define timeTillWiggle 250*1.3


const byte PWMR = 3;  // PWM control (speed) for motor right
const byte PWML = 11; // PWM control (speed) for motor left
const byte DIRR = 12; // Direction control for motor A
const byte DIRL = 13; // Direction control for motor B

int sensorR;
int sensorL;
int sensorFR;
int sensorFL;

#define trigPin 9
#define echoPin 8
#define trigPinF 5
#define echoPinF 4

unsigned long time;

void setup() {
  // put your setup code here, to run once:
  setupArdumoto(); // Set all pins as outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPinF, OUTPUT);
  pinMode(echoPinF, INPUT);
  time = millis();
}





void loop() {
  
  sensorR = analogRead(A5);
  sensorL = analogRead(A4);

  //front sensors for block detection
  sensorFR = analogRead(A2);
  sensorFL = analogRead(A3);

  //printSensors(sensorL, sensorR);
  //Serial.println(ping());

  /*// TESTING FRONT SENSORS

  printSensors(sensorFR,sensorFL);
  delay(200);


  
  // TESTING FRONT SENSORS*/

  driveFWD(SPEED);

  //both sensors activated
  if(checkSensor(sensorR) && checkSensor(sensorL))
  {
    driveREV(SPEED);

  //right sensor activated
  } else if (checkSensor(sensorR) || (ping() > FRONT_BUMPER))
  {
    stopALL();
    drive(MOTOR_L,FWD,SPEED);
    drive(MOTOR_R,REV,SPEED);
    //??degrees want 180+-30?
    delay(500+OFFSET);

  // left sensor
  } else if (checkSensor(sensorL) || (ping() > FRONT_BUMPER))
  {
   stopALL();
   drive(MOTOR_R,FWD,SPEED);
   drive(MOTOR_L,REV,SPEED); 
   //??degrees  want 180+-30?
   delay(500+OFFSET);
  }
  

  //Time to wiggle
  /*if (millis() - time > timeTillWiggle) 
  {
    wiggle(); // stops wihen block is found
    time = millis();
  }*/
  
  checkForBlock();
  
  
}

//return 1 if off table
int checkBottomSensors(){

  updateAllSensors();
  if (checkSensor(sensorR) || (ping() > FRONT_BUMPER) || checkSensor(sensorL))
  {
    return 1;
  } else {
    return 0;
  }
}

void checkForBlock(){
  int dist = pingAVG(10);
  updateAllSensors();
  if ((dist < 130 && dist > 0) || checkSensorF(sensorFL) == 1 || checkSensorF(sensorFR) == 1) {
    Serial.println(dist);
    drive(MOTOR_L,REV,SPEED);
    drive(MOTOR_R,REV,SPEED);
    delay(100);
    stopALL();
    delay(1000);
    updateAllSensors();
    drive(MOTOR_L,REV,SPEED);
    drive(MOTOR_R,REV,SPEED);
    delay(300);
    stopALL();
    delay(1000);
    

    if(checkSensorF(sensorFL) == 1 && checkSensorF(sensorFR) == 1){
      //push to the edge
      Serial.println("Push to the edge");
      while(!checkBottomSensors()){
        drive(MOTOR_L,FWD,SPEED);
        drive(MOTOR_R,FWD,SPEED);
      }
      drive(MOTOR_L,REV,SPEED);
      drive(MOTOR_R,REV,SPEED);
      delay(100);
    }else{
      //turn away
      Serial.println("turning away from block");
      drive(MOTOR_L,FWD,SPEED);
      drive(MOTOR_R,REV,SPEED);
    }
    //??degrees want 180+-30?
    delay(200);
  }
  stopALL();
}

int pingAVG(int sample) {
  int total = 0;
  int sampleSize = sample;
  for (int i = 0; i < sampleSize; i++) {
    total += pingF();
  }
  //Serial.println(total/sampleSize);
  return total/sampleSize;
}

void wiggle()
{
  Serial.println("Wiggling");
  stopALL();
  drive(MOTOR_L,FWD,wiggleSPEED);
  drive(MOTOR_R,REV,wiggleSPEED);
  delay(wiggleTURN);
  if (checkSensorF(sensorFL) == 1 || checkSensorF(sensorFR) == 1)
  {
    return;
  }
  
  stopALL();
  drive(MOTOR_R,FWD,wiggleSPEED);
  drive(MOTOR_L,REV,wiggleSPEED); 
  delay(wiggleTURN*2);
  if (checkSensorF(sensorFL) == 1 || checkSensorF(sensorFR) == 1)
  {
    return;
  }

  
  stopALL();
  drive(MOTOR_L,FWD,wiggleSPEED);
  drive(MOTOR_R,REV,wiggleSPEED);
  delay(wiggleTURN);
  
  
  return;
}

int ping()
{
  int distance;
  int duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  if (duration > FRONT_BUMPER)
  {
    
    //Serial.println(duration);
    
  }
  return duration;
}

int pingF()
{
  int distance;
  int duration;
  digitalWrite(trigPinF, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPinF, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinF, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPinF, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  return duration;
}


int checkSensorF(int sensor)
{
  if(sensor < SENSOR_ACTIVATE_LEVEL_F)
  {
    return 1;
  }

  return 0;
}

void foundBlock()
{
  //Serial.println("Block FOUND");
  //starts from the left
  updateAllSensors();
  if (checkSensorF(sensorFL) == 1)
  {
    Serial.println("Block found on the Left");
    updateAllSensors();
    while (sensorFL > blockDistance)
    {
      Serial.print("moving closer to block (left) ");
      Serial.println(sensorFL);
      driveFWD(SPEED/1.5);
      
      if (checkSensorF(sensorFR) == 1)
      {
        
        while(checkSensor(sensorL) == 0 || checkSensor(sensorR) == 0)
        {
          Serial.println("Driving block off the edge");
          driveFWD(SPEED);
          updateAllSensors();
        }      
      }
      updateAllSensors();
    }
    
  }
  // starts from the right
  else if (checkSensorF(sensorFR) == 1) 
  {
    Serial.println("Block found on the Right");
    updateAllSensors();
    while (sensorFR  > blockDistance)
    {
      Serial.println("moving closer to block (right)");
      driveFWD(SPEED/1.5);
      if (checkSensorF(sensorFL) == 1)
        {
          while(checkSensor(sensorL) == 0 || checkSensor(sensorR) == 0)
          {
            Serial.println("Driving block off the edge");
            driveFWD(SPEED);
            updateAllSensors();
          }
        }
        updateAllSensors();
     }
  }
  return;
}

void updateAllSensors() {
  sensorR = analogRead(A5);
  sensorL = analogRead(A4);

  //front sensors for block detection
  sensorFR = analogRead(A2);
  sensorFL = analogRead(A3);
}

int checkSensor(int sensor)
{
  if(sensor > SENSOR_ACTIVATE_LEVEL)
  {
    return 1;
  }

  return 0;
}

void driveFWD(int speed)
{
  drive(MOTOR_R,FWD,speed);
  drive(MOTOR_L,FWD,speed);
}

void driveREV(int speed)
{
  drive(MOTOR_R,REV,speed);
  drive(MOTOR_L,REV,speed);
}

void printSensors(int left, int right)
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
