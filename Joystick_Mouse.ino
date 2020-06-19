/* 버튼 코드 참조: https://kikiai.tistory.com/29
   마우스 코드 참조: https://www.arduino.cc/reference/ko/language/functions/usb/mouse/mousemove/
   위의 링크에 마우스 관련 함수 다 설명되있음. 
   조이스틱 코드 참조: http://blog.naver.com/PostView.nhn?blogId=boilmint7&logNo=220927175653
   조이스틱에도 버튼이 있지만 사용하기 힘들어서 안썼음.

   버튼: 핀 4개 있는데 2개 버리고 2개중 하나에 +5V, 하나는 그라운드랑 Pin2에 박으면 됨.
   
   조이스틱: 그라운드랑 +5V는 말 그대로 그라운드랑 +5V에 연결 
   VRX, VRY 는 각각 A0, A1에 연결함. 나는 조이스틱핀이 아래를 보는 방향으로 구현해서 x, y축이 바뀜.
   그래서 아래 코드 보면 A0, A1 순서가 다름. 
*/


#include <Mouse.h> // 마우스 헤더(레오나르도가 아니면 라이브러리 깔아도 오류 뜸!, 무조건 레오나르도로 ㄱㄱ)

void setup() {
  pinMode(2, INPUT); // 좌클릭 버튼
  pinMode(4, INPUT); // 우클릭 버튼
  Mouse.begin(); // 마우스 제어 시작
  Serial.begin(9600); //시리얼 통신을 시작
}

void loop() {/* 다 다르겠지만 나는 조이스틱 초기 값이 x: 517, y: 506 정도였음. 이게 의미하는건 
                중심 좌표가 (512, 512) 쯤이라는건데, 사용하기 불편하므로 중심을 (0, 0)로 만들었다. 
                200을 나눈건 마우스가 너무 큰 범위로 움직이게 되기 때문에 크기를 줄여줬다.*/
  int x = (analogRead(A1) - 517) / 260; /* 나누는 값은 맘대로 바꿔도 된다. 다만 500이 넘어가면 마우스가 움직이지 않게 된다.
                                         왜냐하면 (analogRead(A1) - 517)의 최대값이 500정도 되는데, 거기에 500을 나눠버리면 0이 되기 때문*/
  int y = - (analogRead(A0) - 506) / 260; /* 조이스틱을 위로 올리면 같이 +가 되야 되는데,
                                             -가 되길래 그냥 -곱해줬다.*/
  int Left_button = digitalRead(2); // 1이면 누른 상태, 0이면 안 누른 상태임.
  int Right_button = digitalRead(4); // 위와 마찬가지
  Sprint(x, y, Left_button, Right_button); // 시리얼 프린트
  
  if (Left_button == 1) // Left button이 1이라는건 좌클릭을 하고 있다는 상태이므로 마우스를 누른 상태로 유지시켜주는 Mouse.press()를 사용함
  {
    Mouse.press(MOUSE_LEFT); // 좌클릭 한 상태 유지
  }
  else if (Left_button == 0 && Right_button == 0) 
  {
    Mouse.release(); //클릭 상태 해제
  }
  else if (Right_button == 1) // 우클릭
  {
    Mouse.click(MOUSE_RIGHT);
  } 
  Mouse.move(x, y); // 마우스를 움직이는 함수
}

// 루프안에 여러줄 있는거 꼴보기 싫어서 함수로 만듬. 별 의미 없음.
void Sprint(int x, int y, int lb, int rb) 
{
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(' ');
  Serial.print("Y: ");
  Serial.print(-y); // 시리얼프린트에 -로 출력되길래 부호 바꿔줌.
  Serial.print(' ');
  Serial.print("Left_button: ");
  Serial.print(lb);
  Serial.print(' ');
  Serial.print("Rigft_button: ");
  Serial.println(rb);
}
