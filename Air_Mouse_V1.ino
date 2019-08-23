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

//민감도 조절 변수
int timer1 = 0;
int timer2 = 0;
int sens = 1100; // sens_num = 3
int sens_num = 3; // sens = 1100;
int b_cnt = 0;

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

  int lb = digitalRead(4); // 1이면 누른 상태, 0이면 안 누른 상태임.
  int rb = digitalRead(8); // 위와 마찬가지
  
  int gyroY, gyroZ;
  
  //감도조절 감도조절 감도조절 감도조절 감도조절 감도조절 감도조절 감도조절
  
  if (lb == 1 && rb == 1) 
  {
    ++timer1;
    if (timer1 > 1000) 
    {   
      Serial.println("민감도 설정 모드\n");
      delay(1200);
      Serial.print("민감도:  ");
      Serial.println(sens_num);
      while(1) 
      { 
        lb = digitalRead(4);
        rb = digitalRead(8);
        //led나 부저 on 시키는 구문 추가
        
        if(lb == 1 && rb == 1) 
        {
          ++timer2 ;
        }        
        else 
        { 
          b_cnt += 1;
          timer2 = 0;
        }
        
        if(lb == 1) 
        { 
          if(b_cnt > 100 && sens_num > 1) 
          { 
            --sens_num; //분모가 작아지니까 gyro값 커짐 >> 민감도 커짐   
            b_cnt = 0;
            Serial.print("민감도:  ");
            Serial.println(sens_num);
            delay(300);
          }
        }
        else if(rb == 1) 
        {
          if(b_cnt > 100 && sens_num < 5) 
          { 
            ++sens_num; //분모가 커지니까 gyro값 작아짐 >> 민감도 작아짐  
            b_cnt = 0;
            Serial.print("민감도:  ");
            Serial.println(sens_num);
            delay(300);
          }    
        }
        
        switch(sens_num) 
        {
           case 1: sens = 900; break;
           case 2: sens = 990; break;
           case 3: sens = 1100; break;
           case 4: sens = 1220; break;
           case 5: sens = 1380; break;
           default: break;
        }
        
        if(timer2 > 1700) 
        {
          break;
        }
        
        //Sprint2(timer1, timer2, sens, sens_num);
      }
      timer1 = 0;
      timer2 = 0;
      Serial.println("\n민감도 설정 완료!");  
      Serial.print("현재 민감도: "); 
      Serial.println(sens_num);      
      delay(3000);   
    }
  }
  
  else 
  {
    timer1 = 0;
  }
  
  gyroZ = GyZ / sens;
  gyroY = -GyY / sens * 1.22; 

  if(lb == 1) // Left button이 1이라는건 좌클릭을 하고 있다는 상태이므로 
  {                                       // 마우스를 누른 상태로 유지시켜주는 Mouse.press()를 사용함.
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

  Sprint1(gyroY, gyroZ, lb, rb);
  //Sprint2(timer1, timer2, sens, sens_num);
  Mouse.move(gyroZ, gyroY); // 마우스를 움직이는 함수
}

void Sprint1(int gyroY, int gyroZ, int lb, int rb) //루프안에 여러줄 있는거 꼴보기 싫어서 함수로 만듬.
{
  Serial.print(" gyroY = "); 
  Serial.print(gyroY);
  Serial.print(" gyroZ = "); 
  Serial.print(gyroZ);
  Serial.print(" Left_button: ");
  Serial.print(lb);
  Serial.print(" Rigft_button: ");
  Serial.println(rb);
}

void Sprint2(int timer1, int timer2, int sens, int sens_num) //루프안에 여러줄 있는거 꼴보기 싫어서 함수로 만듬.
{
  Serial.print("timer1: ");
  Serial.print(timer1);
  Serial.print("  ");
  Serial.print("timer2: ");
  Serial.print(timer2);
  Serial.print("  ");
  Serial.print("sens: ");
  Serial.print(sens);
  Serial.print("  ");
  Serial.print("sens_num: ");
  Serial.println(sens_num);
}
