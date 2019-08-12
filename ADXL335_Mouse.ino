#include<Mouse.h> // 마우스 움직여야지!

const int xpin = A3;                  // x-axis
const int ypin = A2;                  // y-axis
const int zpin = A1;                  // z-axis (only on 3-axis models)

void setup()
{
  pinMode(2, INPUT); // 좌클릭 버튼
  pinMode(4, INPUT); // 우클릭 버튼
  Mouse.begin();
  Serial.begin(9600);
}

void loop()
{
  float ax = (analogRead(xpin) - 330) / 20;
  float ay = -(analogRead(ypin) - 330) / 20;
  float az = (analogRead(zpin) - 330) / 20;

  int Left_button = digitalRead(2); // 1이면 누른 상태, 0이면 안 누른 상태임.
  int Right_button = digitalRead(4); // 위와 마찬가지

  Sprint(ax, ay, az, Left_button, Right_button);

  if (Left_button == 1) /* Left button이 1이라는건 좌클릭을 하고 있다는 상태이므로 마우스를
                          누른 상태로 유지시켜주는 Mouse.press()를 사용함*/
  {
    Mouse.press(); // 좌클릭 한 상태 유지
  }
  else if (Left_button == 0 && Right_button == 0)
  {
    Mouse.release(); //클릭 상태 해제
  }
  else if (Right_button == 1) // 우클릭
  {
    Mouse.click(MOUSE_RIGHT);
  }
  Mouse.move(ay, ax); // 마우스를 움직이는 함수
  delay(1);
}

void Sprint(int ax, int ay, int az, int lb, int rb)
{
  Serial.print("ax: ");
  Serial.print(ax);
  Serial.print(" ay: ");
  Serial.print(ay);
  Serial.print(" az: ");
  Serial.print(az);
  Serial.print(" lb: ");
  Serial.print(lb);
  Serial.print(" rb: ");
  Serial.println(rb);
}
