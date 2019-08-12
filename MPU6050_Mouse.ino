#include<Wire.h> // 통신? 때문에 필요
#include<Mouse.h> // 마우스 움직여야지!


/*
 SDA 2번 Pin, SCL 3번 Pin
 Int 7번 Pin
 Int 무조건 박아야됨. 
 */


#define MPU6050_INT_PIN 7
#define MPU6050_INT digitalPinToInterrupt(MPU6050_INT_PIN)

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int sensitivity = 2000; /* 출력 값이 엄청 커서 그대로 Mouse.move로 들어가면 마우스 움직임 폭이 너무 넓어서 조절이 안됨.
                             그래서 큰 숫자를 나눠줌. 딱 쓰기 편할 정도로 조절해야 할 듯. 숫자 바꿔가면서 노가다 ㄱㄱ*/
                             
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
  pinMode(4, INPUT); // 좌클릭 버튼
  pinMode(8, INPUT); // 우클릭 버튼
  Mouse.begin(); //넣어야 되는줄 알았더니 이거 넣으니까 계속 업로드가 안되는거 같음. 일단 지우고 하면 잘되니까 냅두자
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

  int lb = digitalRead(4); // 1이면 누른 상태, 0이면 안 누른 상태임.
  int rb = digitalRead(8); // 위와 마찬가지
  
  int gyroX = - GyX / 2000; /* 내가 연결한 방향은 마이너스를 해야지 원하는 방향으로 움직임. 연결방식마다 다르니 자기 
                                 께 반대로 움직인다 싶으면 부호를 바꿔주자*/  
  int gyroZ = - GyZ / 1800; // 위와 마찬가지

  if (lb == 1) // Left button이 1이라는건 좌클릭을 하고 있다는 상태이므로 마우스를 누른 상태로 유지시켜주는 Mouse.press()를 사용함
  {
    Mouse.press(); // 좌클릭 한 상태 유지
  }
  else if (lb == 0 && rb == 0) 
  {
    Mouse.release(); //클릭 상태 해제
  }
  else if (rb == 1) // 우클릭
  {
    Mouse.click(MOUSE_RIGHT);
  } 
  
  Sprint(gyroX, gyroZ, lb, rb);
  Mouse.move(gyroZ, gyroX); // 마우스를 움직이는 함수
}

void Sprint(int gyroX, int gyroZ, int lb, int rb) // 루프안에 여러줄 있는거 꼴보기 싫어서 함수로 만듬. 별 의미 없음.
{
  Serial.print("gyroX = "); 
  Serial.print(gyroX);
  Serial.print(" gyroZ = "); 
  Serial.print(gyroZ);
  Serial.print(" Left_button: ");
  Serial.print(lb);
  Serial.print(" Rigft_button: ");
  Serial.println(rb);
}
