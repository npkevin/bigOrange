
#define REV 0
#define FWD 1

#define MOTOR_R 0
#define MOTOR_L 1
#define SPEED 100//100isgood
#define OFFSET 0
#define SENSOR_ACTIVATE_LEVEL 950
#define FRONT_BUMPER 800





const byte PWMR = 3;  // PWM control (speed) for motor right
const byte PWML = 11; // PWM control (speed) for motor left
const byte DIRR = 12; // Direction control for motor A
const byte DIRL = 13; // Direction control for motor B

int sensorR;
int sensorL;
int sensorF;

#define trigPin 5
#define echoPin 4

unsigned long time;

void setup() {
  // put your setup code here, to run once:
  setupArdumoto(); // Set all pins as outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
}





void loop() {
  
  sensorR = analogRead(A5);
  sensorL = analogRead(A4);

  //front sensors for block detection
  sensorF = analogRead(A2);//a3 might be setup

  //printSensors(sensorL, sensorR, sensorF);
  //Serial.println(ping());

  driveFWD(SPEED);

  //both sensors activated
  if(checkSensor(sensorR) && checkSensor(sensorL))
  {
    driveREV(SPEED);

  //right sensor activated
  } else if (checkSensor(sensorR))
  {
    stopALL();
    drive(MOTOR_L,FWD,SPEED);
    drive(MOTOR_R,REV,SPEED);
    //??degrees want 180+-30?
    delay(500+OFFSET);

  // left sensor
  } else if (checkSensor(sensorL))
  {
   stopALL();
   drive(MOTOR_R,FWD,SPEED);
   drive(MOTOR_L,REV,SPEED); 
   //??degrees  want 180+-30?
   delay(500+OFFSET);
  }

  
  //avoid squid
  /*if ((ping() < FRONT_BUMPER)) {
    stopALL();
    //turnaway //copied front above
    drive(MOTOR_R,FWD,SPEED);
    drive(MOTOR_L,REV,SPEED); 
    //??degrees  want 180+-30?
    delay(400+OFFSET);
  }*/
  int i;
  updateAllSensors();
  if(sensorF > 700){
    Serial.println("Found line!!");
    stopALL();
    driveFWD(SPEED/1.8);
    delay(200);
    updateAllSensors();
    if(sensorF < 600){
      stopALL();
      Serial.println("Finding line!");
      //turn right then left
      time = millis();
      long unsigned startTime = 0;
      for (i=0;i<3;i++) {
        updateAllSensors();
        Serial.println(i);
        int ret = findLine(300+300*i, 1.3, startTime);
        if(ret ==1)
          break;
      }
    }
  }
  
}

//findline
//return 1 if line found, 0 otherwise
int findLine(int turntime, float x, long unsigned startTime) {
  
      while(sensorF < 600 && startTime<turntime){
        updateAllSensors();
        Serial.println("Turning right!");
        //turnaway //copied front above
        drive(MOTOR_R,REV,SPEED/x);
        drive(MOTOR_L,FWD,SPEED/x); 
        startTime = millis()-time;
      }
      stopALL();
      if(sensorF < 600){
        
        long unsigned startTime = 0;
        while(sensorF < 600 && startTime<turntime*2){
          Serial.println("Turning left!");
          updateAllSensors();
          //turnaway //copied front above
          drive(MOTOR_R,FWD,SPEED/x);
          drive(MOTOR_L,REV,SPEED/x); 
          startTime = millis()-time;
        }
        stopALL();
      }
      if (sensorF > 700){
        return 1;
      }else{
        return 0;
      }
      
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





void updateAllSensors() {
  sensorR = analogRead(A5);
  sensorL = analogRead(A4);

  //frontbottom
  sensorF = analogRead(A2);
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

void printSensors(int left, int right, int front)
{
  
  
  Serial.print("left: ");
  Serial.print(left);
  Serial.println();
  Serial.print("right: ");
  Serial.print(right);
  Serial.println();
  Serial.print("Front: ");
  Serial.print(front);
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
