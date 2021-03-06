/*
Author: Vlad Silin
Description: A prototype Arduino sketch which implements the PID
(Proportional-Integral-Derivative) controller algorithm
*/

int enablePin1 = 3;
int enablePin2 = 9;
int rightMotorForward = 7;
int rightMotorReverse = 8;
int leftMotorForward = 12;
int leftMotorReverse = 2;
int maxSpeed = 60;
int rightSpeed;
int leftSpeed;

int setPoint = 0;
long sensorsAverage;
long sensorsSum;
long pos;
int errorValue;
int lighting = 4;
long sensors[] = {
  0, 0, 0, 0, 0}; 
long sensorMax[] = {
  0, 0, 0, 0, 0};
long sensorMin[] = {
 1023, 1023, 1023, 1023, 1023};

void setup()
{ 
  Serial.begin(9600);
  int summation = 0;
  int counter = 0;
  pinMode(lighting, OUTPUT );
  digitalWrite (lighting, HIGH);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  
  while (millis () < 7000)
  {
    for (int i = 0; i < 5; i++)
    {
      sensors [i] = analogRead(i);
      if (sensors[i] > sensorMax[i]) {
        sensorMax[i] = sensors [i];
      }
      if (sensors[i] < sensorMin[i]) {
        sensorMin[i] = sensors [i];
      }
    } 
  }
  
  digitalWrite(13, LOW);
  delay (5000);
  digitalWrite(13, HIGH);
  while (counter < 25) {
    for (int i = -20; i <= 20; i = i+10)
    {
      sensors[i/10+2] = analogRead(i/10+2);
      sensors [i/10+2] = map(sensors[i/10+2], sensorMin[i/10+2], sensorMax[i/10+2], 0, 255);
      sensors [i/10+2] = constrain(sensors [i/10+2], 0, 255);
      sensorsAverage += sensors[i/10+2] * i * 1000; 
      sensorsSum += long(sensors[i/10+2]);
    } 
    pos = long(sensorsAverage / sensorsSum);
    summation += pos;
    Serial.println(pos);
    Serial.println(summation);
    counter++;
  }
  setPoint = summation/counter;
  Serial.println(setPoint); 
  
  digitalWrite(13, LOW);
  
  pinMode(lighting, OUTPUT );
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorReverse, OUTPUT);
  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorReverse, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(enablePin2, OUTPUT);
}

void loop()
{ 
  sensorsRead ();
  PidCalc();
  calcTurn();
  motorDrive(rightSpeed, leftSpeed);
}

void sensorsRead ()
{
  sensorsAverage = 0;
  sensorsSum = 0;
  pos = 0;

  for (int i = -20; i <= 20; i = i+10)
  {
    sensors[i/10+2] = analogRead(i/10+2);
    sensors [i/10+2] = map(sensors[i/10+2], sensorMin[i/10+2], sensorMax[i/10+2], 0, 255);
    sensors [i/10+2] = constrain(sensors [i/10+2], 0, 255);
    sensorsAverage += sensors[i/10+2] * i * 1000;
    sensorsSum += int(sensors[i/10+2]);
  }

  pos = int(sensorsAverage / sensorsSum);
  Serial.print(sensorsAverage);
  Serial.print(' ');
  Serial.print(sensorsSum);
  Serial.print(' ');
  Serial.print(pos);
  Serial.println();
  delay(2000); 
}

void PidCalc()
{ 
  int integral = 0;
  int proportional;
  int derivative;
  int lastProportional = 0;
  float Kp = 0.6;
  float Ki = 0;
  float Kd = 0;

  pos = int(sensorsAverage / sensorsSum);
  Serial.println (setPoint);
  proportional = pos-setPoint;
  integral = integral + proportional;
  derivative = proportional - lastProportional;
  lastProportional = proportional;
  errorValue = int(proportional * Kp + integral * Ki + derivative * Kd);
}

void calcTurn()
{ 
  errorValue = constrain(errorValue, -256, 256);
  
  if (errorValue < 0)
  {
    leftSpeed = maxSpeed;
    rightSpeed = maxSpeed - errorValue;
  }
  else if (errorValue > 0)
  {
    rightSpeed = maxSpeed;
    leftSpeed = maxSpeed - errorValue;
  }
  else
  {
    rightSpeed = maxSpeed;
    leftSpeed = maxSpeed;
  }
}

void motorDrive(int rightSpeed, int leftSpeed)
{ 
  analogWrite(enablePin1, rightSpeed);
  analogWrite(enablePin2, leftSpeed);
  digitalWrite (rightMotorForward, HIGH);
  digitalWrite (leftMotorForward, HIGH);
  delay(50); 
}






