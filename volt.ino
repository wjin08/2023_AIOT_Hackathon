// 로봇에 인가된 배터리를 측정하는 로직
int analogval = 0;
float voltage = 0;

void setup() {
  pinMode(4, OUTPUT); //파
  pinMode(5, OUTPUT);//녹
  pinMode(6, OUTPUT);//적
  Serial.begin(9600); // 시리얼 통신 초기화
}

void loop() {
  analogval = analogRead(A0);
  voltage = 0.0048 * analogval;

  if (voltage > 6.4) {
    digitalWrite(4, HIGH);
    delay(1000);
    digitalWrite(4, LOW);
  } else if (voltage < 6.4 && voltage > 5.6) {
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);
  } else if (voltage <= 5.6) //전압이 5.6v 이하일시 븕은 led 점등을 한다
  {
    digitalWrite(6, HIGH);
    delay(1000);
    digitalWrite(6, LOW);
  }

  // 시리얼 모니터에 값을 출력
  Serial.print("Analog Value: ");
  Serial.print(analogval);
  Serial.print(", Voltage: ");
  Serial.println(voltage, 2); // 소수점 이하 두 자리까지 출력
}
