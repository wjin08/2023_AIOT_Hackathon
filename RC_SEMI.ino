#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // 블루투스 TX, RX

char command; // 블루투스로부터 수신
int stance = 0; // 상태를 나타내는 변수

//#define RED_LED_PIN ? // 빨강 LED 핀 번호
//#define GREEN_LED_PIN ?  // 초록 LED 핀 번호
//#define YELLOW_LED_PIN ? // 노랑 LED 핀 번호


#define trigPin 2// 첫 번째 초음파 센서 트리거 핀 
#define echoPin 3 // 첫 번째 초음파 센서 에코 핀 
//#define trigPin2 ?? // 두 번째 초음파 센서 트리거 핀 
//#define echoPin2 ?? // 두 번째 초음파 센서 에코 핀


// GET DISTANCE

float getDistance(int trig, int echo)
{
  float duration; 
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  return (duration / 2) / 29.1;
}

float getMeanDist(int trig, int echo)
{
  float dist1 = getDistance(trig, echo);
  float dist2 = getDistance(trig, echo);
  return (dist1 + dist2) / 2;
}

// GET VOLTAGE

float getVoltage(int analog)
{
  return analogRead(analog) * (5.0 / 1023.0);
}

// AUTONOMOUS DRIVING
#define ENA 11 // 속도
#define IN1 10 // 1,2는 A포트
#define IN2 9
#define IN3 8
#define IN4 7
#define ENB 6

int tick = 1;
int duration = 0;

typedef enum {CALL = 0, PARK, NORMAL, SLIGHT_LEFT, SLIGHT_RIGHT, TURN_LEFT, TURN_RIGHT, BACKUP} MODE;
typedef enum {STOP = 0, SLOW = 200, FAST = 255} SPEED;
int drivemode = NORMAL;
int prevmode;

void AD_changeMode()
{
  prevmode = drivemode;
  float dist = getMeanDist(trigPin, echoPin);
  Serial.println(dist);
  Serial.println(drivemode);
  if ((drivemode == PARK || drivemode >= TURN_LEFT) && duration == 0) drivemode = random(2, 5);
  if (drivemode >= NORMAL && drivemode <= SLIGHT_RIGHT && tick % random(30, 60) == 0) drivemode = random(2, 5);
  if (drivemode >= NORMAL && drivemode <= SLIGHT_RIGHT && tick % random(30, 60) == 0)
  {
    drivemode = random(5, 7);
    duration = random(10, 15);
  }
  if (tick % random(50, 100) == 0)
  {
    drivemode = PARK;
    duration = random(5, 7);
  }
  if (dist < 30)
  {
    drivemode = BACKUP;
    duration = random(3, 5);
  }
}

void AD_setSpeed()
{
  switch (drivemode)
  {
    case CALL:
    case PARK:
    setSpeed(true, true, STOP, STOP);
    break;
    case NORMAL:
    setSpeed(true, true, FAST, FAST);
    break;
    case SLIGHT_LEFT:
    setSpeed(true, true, SLOW, FAST);
    break;
    case SLIGHT_RIGHT:
    setSpeed(true, true, FAST, SLOW);
    break;
    case TURN_LEFT:
    setSpeed(true, true, STOP, FAST);
    break;
    case TURN_RIGHT:
    setSpeed(true, true, FAST, STOP);
    break;
    case BACKUP:
    switch (prevmode)
    {
      case SLIGHT_LEFT:
      setSpeed(false, false, SLOW, FAST);
      break;
      case SLIGHT_RIGHT:
      setSpeed(false, false, FAST, SLOW);
      break;
      case TURN_LEFT:
      setSpeed(false, false, STOP, FAST);
      break;
      case TURN_RIGHT:
      setSpeed(false, false, FAST, STOP);
      break;
      case NORMAL:
      default:
      setSpeed(false, false, FAST, FAST);
      break;
    }
    break;
  }
}

void setSpeed(bool ld, bool rd, int ls, int rs) // true: forward, false: backward
{
  if (ld)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  if (rd)
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  else
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  analogWrite(ENA, ls);
  analogWrite(ENB, rs);
}

// CALL MODE

void CALL_forward()
{
  if (drivemode == CALL)
  {
    setSpeed(true, true, FAST, FAST);
    delay(2000);
    setSpeed(true, true, STOP, STOP);
  }
}

void CALL_rotate()
{
  if (drivemode == CALL)
  {
    setSpeed(true, false, FAST, FAST);
    delay(1000);
    setSpeed(true, true, STOP, STOP);
  }
}


// SETUP

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
/*  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);*/

}
/*
void run() { // 전진
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void brake() { // 제동, 정지
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void reverse() {
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() { // 좌회전 (좌측정지, 우측직진)
  analogWrite(ENA, sp);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}

void right() { // 우회전 (우측정지, 좌측직진)
  analogWrite(ENA, sp);
  analogWrite(ENB, 0);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void controlLEDs(float distance2, float batteryVoltage) 
{
  if (batteryVoltage < 5.6) 
  {
    digitalWrite(RED_LED_PIN, HIGH);
    Serial.println("Battery is running low.");
  } 
  else 
  {
    digitalWrite(RED_LED_PIN, LOW);
    if (stance == 1) 
    {
      if (distance2 < 20 && batteryVoltage >= 5.6) 
      {
        digitalWrite(YELLOW_LED_PIN, HIGH);
        Serial.println("Trashbin is fully loaded");
      } 
      else 
      {
        digitalWrite(YELLOW_LED_PIN, LOW);
      }
      if (distance2 < 20 && batteryVoltage >= 5.6) 
      {
        digitalWrite(GREEN_LED_PIN, HIGH);
      } else 
      {
        digitalWrite(GREEN_LED_PIN, LOW);
      }
    }
  }
}
*/
void loop() 
{
  //controlLEDs(getDistance(trigPin2, echoPin2), getVoltage(A0)); //LED통제

  AD_changeMode();
  AD_setSpeed();
  tick++;
  if (duration > 0) duration--;
}

