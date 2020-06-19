#include<Wire.h> // 통신 때문에 필요
#include<Mouse.h> // 마우스 움직여야지!

/*
 SDA 2번 Pin, SCL 3번 Pin, Int 7번 Pin
 Int 무조건 박아야됨. 
*/
 
#define MPU6050_INT_PIN 7 // 레오나르도에서는 무조건 7번 Pin에 박아주자
#define MPU6050_INT digitalPinToInterrupt(MPU6050_INT_PIN)

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

//민감도 조절 변수
int timer1 = 0;
int sens = 1100; // sens_num = 3
int sens_num = 3; // sens = 1100;

int Led1 = 9;
int Led2 = 10;
int Led3 = 11;
int Led4 = 12;
int Led5 = 13;

void setup()
{
  // 설정인데 안 건드는게 좋음. 잘 되있음.
  Serial.begin(9600);
  Serial.println("start");
  Serial.print(MPU6050_INT); Serial.print(" on pin "); Serial.print(MPU6050_INT_PIN);
  
  Mouse.begin();

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  pinMode(4, INPUT); // 좌클릭 버튼
  pinMode(8, INPUT); // 우클릭 버튼
  pinMode(MPU6050_INT, INPUT);
  
  //attachInterrupt(MPU6050_INT, dmpDataReady, RISdlfhgk62ING);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(Led4, OUTPUT);
  pinMode(Led5, OUTPUT);
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

  // 버튼의 센서값을 읽음 -> 누르면 1, 안 누르면 0
  int lb = digitalRead(4); // 좌클릭, 1이면 누른 상태, 0이면 안 누른 상태임.
  int rb = digitalRead(8); // 우클릭, 위와 마찬가지
  
  float gyroY, gyroZ;
  
  // 좌우클릭을 동시에 함 
  if (lb == 1 && rb == 1) {
    ++timer1;
    delay(20);
    // 1초 동안 동시에 좌우클릭 -> 민감도 설정 모드
    if (timer1 > 50) {
      Serial.println("민감도 설정 모드\n");
      delay(500); // 이거 없으면 민감도 설정 모드 바로 탈출하게 됨. 꼭 필요함.
      Serial.print("민감도:  ");
      Serial.println(sens_num);
      
      while(1) { 
        lb = digitalRead(4); // 좌클릭 
        rb = digitalRead(8); // 우클릭

        // 민감도 설정 모드 안에서 한번 더 좌우 클릭을 동시에 한 경우 
        if(lb == 1 && rb == 1) { 
          break; // 민감도 설정 모드 탈출 
        }        
        
        else if(lb == 0 && rb == 1) { 
          if(sens_num > 1) { 
            --sens_num; //분모가 커지니까 gyro값 작아짐 >> 민감도 작아짐  
            Serial.print("민감도:  ");
            Serial.println(sens_num);
            delay(500);
          }
        }
        
        else if(lb == 1 && rb == 0) {
          if(sens_num < 5) { 
            ++sens_num; //분모가 작아지니까 gyro값 커짐 >> 민감도 커짐
            Serial.print("민감도:  ");
            Serial.println(sens_num);
            delay(500);
          }    
        }
        
        switch(sens_num) {
           case 5: sens = 820; Sens_Led_On(sens_num); break;
           case 4: sens = 850; Sens_Led_On(sens_num); break;
           case 3: sens = 1100; Sens_Led_On(sens_num); break;
           case 2: sens = 1220; Sens_Led_On(sens_num); break;
           case 1: sens = 1380; Sens_Led_On(sens_num); break;
           default: break;
        }
        //Sprint2(timer1, sens, sens_num);
      }
      timer1 = 0;
      Serial.println("\n민감도 설정 완료!");
      Serial.print("현재 민감도: ");
      Serial.println(sens_num);     
      delay(500);
    }
  }
  // 좌우클릭을 1초 동안 하지 않은 경우
  else { 
    timer1 = 0; // timer1 변수 값 0으로 초기화
  }
  
  gyroZ = GyZ / sens;  // X축
  gyroY = -GyY / sens; // Y축
  
  // lb 가 1 -> 좌클릭을 한 경우
  if(lb == 1)  {
    // 마우스를 누른 상태로 유지시켜주는 Mouse.press()를 사용함.           
    Mouse.press(); // 좌클릭 한 상태 유지
  }

  // rb 가 1 -> 우클릭을 한 경우
  else if (rb == 1) {
    Mouse.click(MOUSE_RIGHT); // 우클릭
    delay(200);
  } 

  // 좌우클릭 둘 다 안했으므로 클릭 상태 해제
  else if (lb == 0 && rb == 0) {
    Mouse.release(); //클릭 상태 해제
  }

  Sprint1(gyroY, gyroZ, lb, rb); // 센서 값을 출력해줌
  //digitalWrite(8 + sens_num, HIGH); // 민감도에 대응되는 LED를 킴
  Mouse.move(gyroZ, gyroY); // 마우스를 움직여주는 함수
}

// 민감도에 따른 LED On/Off 함수
void Sens_Led_On(int sens_num) 
{
  // 우선 다 끄고 시작함
  digitalWrite(Led1, LOW);
  digitalWrite(Led2, LOW);
  digitalWrite(Led3, LOW);
  digitalWrite(Led4, LOW);
  digitalWrite(Led5, LOW);
  
  /*
  민감도에 대응되는 LED 하나만 켜야 됨.  
  8 + sens_num --> Led1,2,3,4,5 에 대응되는 9, 10, 11, 12, 13번 핀을 의미
  Example) Sens_num이 3일 때 8 + sens_num은 11 즉 Led3을 키게 된다. 
  다른 4가지 경우도 똑같이 작동함. 
  */
  digitalWrite(8 + sens_num, HIGH); // Led On
}

// 센서 값을 출력해줌는 함수(자이로, 버튼)
void Sprint1(int gyroY, int gyroZ, int lb, int rb)
{
  Serial.print("X축: "); 
  Serial.print(gyroZ);
  Serial.print(" // Y축: "); 
  Serial.print(-gyroY);
  Serial.print(" // 좌 클릭: ");
  Serial.print(rb);
  Serial.print(" // 우 클릭: ");
  Serial.println(lb);
}
