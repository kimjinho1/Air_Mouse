#include<Wire.h> // 통신? 때문에 필요
#include<Mouse.h> // 마우스 움직여야지!

/*
 SDA 2번 Pin, SCL 3번 Pin
 Int 7번 Pin
 Int 무조건 박아야됨. 
 */
 
#define MPU6050_INT_PIN 7 // 레오나르도에서는 무조건 박아주자

#define MPU6050_INT digitalPinToInterrupt(MPU6050_INT_PIN)

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void setup()
{
  // 설정인데 안 건드는게 좋음. 잘 되있음.
  Serial.begin(9600);

  Serial.println("start");
  Serial.print(MPU6050_INT); Serial.print(" on pin "); Serial.print(MPU6050_INT_PIN);
  
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  pinMode(10, INPUT); // 좌클릭 버튼
  pinMode(11, INPUT); // 우클릭 버튼
  Mouse.begin();
  pinMode(MPU6050_INT, INPUT);
  //attachInterrupt(MPU6050_INT, dmpDataReady, RISdlfhgk62ING);
}

void loop()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  // 여기 위까지는 코드 건들지 않는 걸 추천. 여기 아래부터 수정 ㄱㄱ

  int lb = digitalRead(10); // 1이면 누른 상태, 0이면 안 누른 상태임.
  int rb = digitalRead(11); // 위와 마찬가지
  
  float gyroY, gyroZ;
  int sens = 1100;
  gyroZ = GyZ / sens;
  gyroY = -GyY / sens;

if(lb == 1) // Left button이 1이라는건 좌클릭을 하고 있다는 상태이므로 
  {                                       // 마우스를 누른 상태로 유지시켜주는 Mouse.press()를 사용함.
    Mouse.press(); // 좌클릭 한 상태 유지
  }
  if (rb == 1) // 우클릭
  {
    Mouse.press(MOUSE_RIGHT);
  } 
  if (lb == 0) 
  {
    Mouse.release(); //클릭 상태 해제
  }
   if (rb == 0) 
  {
    Mouse.release(MOUSE_RIGHT); //클릭 상태 해제
  }

  Sprint1(gyroY, gyroZ, lb, rb);
  Mouse.move(gyroZ, gyroY); // 마우스를 움직이는 함수
}

void Sprint1(int gyroY, int gyroZ, int lb, int rb) // 자이로, 버튼 값 확인용 시리얼 프린트 함수
{
  Serial.print("X축: "); 
  Serial.print(gyroZ);
  Serial.print(" // Y축: "); 
  Serial.print(-gyroY);
  Serial.print(" // 좌 클릭: ");
  Serial.print(lb);
  Serial.print(" // 우 클릭: ");
  Serial.println(rb);
}
