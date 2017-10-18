#define REV 0
#define FWD 1

#define MOTOR_R 0
#define MOTOR_L 1
#define SPEED 100//80isgood
#define OFFSET 0
#define SENSOR_ACTIVATE_LEVEL 950
#define FRONT_BUMPER 1000


const byte PWMR = 3;  // PWM control (speed) for motor right
const byte PWML = 11; // PWM control (speed) for motor left
const byte DIRR = 12; // Direction control for motor A
const byte DIRL = 13; // Direction control for motor B

#define trigPin 9
#define echoPin 8



void setup() {
  // put your setup code here, to run once:
  setupArdumoto(); // Set all pins as outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}





void loop() {
  
  int sensorR = analogRead(A5);fr
  int sensorL = analogRead(A4);

  //printSensors(sensorL, sensorR);
  Serial.println(ping());

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
  } else if (checkSensor(sensorL) || (ping() > FRONT_BUMPER))
  {
   stopALL();
   drive(MOTOR_R,FWD,SPEED);
   drive(MOTOR_L,REV,SPEED); 
   //??degrees  want 180+-30?
   delay(500+OFFSET);
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
    
    Serial.println("TRIGGERED");
    Serial.println("TRIGGERED");
    Serial.println("         ");
    Serial.println("         ");
    Serial.println("         ");
    Serial.println("         ");
    Serial.println("         ");
    Serial.println("         ");
    Serial.println("         ");
    Serial.println("         ");
    Serial.println("TRIGGERED");
    Serial.println("TRIGGERED");
    
  }
  return duration;
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
