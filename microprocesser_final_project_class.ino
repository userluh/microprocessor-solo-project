#include<Servo.h>
#include<PinChangeInterrupt.h>
#include<NDelayFunc.h>

Servo dudergi_1;//1번두더지
Servo dudergi_2;//2번두저지
Servo dudergi_3;//3번두더지

/*세븐세그먼트 핀설정*/
unsigned int led[7]={0,1,2,3,4,5,6};
unsigned int com[1+4]={-1,7,8,12,13};

const unsigned int interrupt_button_switch=A1;//게임스타트 버튼 및 속도조절 인터럽트 버튼
const unsigned int raser_send_pin=A2;//레이저 송신모듈
const unsigned int reaser_recev_pin1=A3;//레이저 수신모듈 1
const unsigned int reaser_recev_pin2=A4;//레이저 수신모듈 2
const unsigned int reaser_recev_pin3=A5;//레이저 수신모듈 3

unsigned long t1_prev=0;
unsigned long t1_delay=700;//1번 두더지 올라오는 시간간격

unsigned long t2_prev=0;
unsigned long t2_delay=950;//2번 두더지 올라오는 시간간격

unsigned long t3_prev=0;
unsigned long t3_delay=1200;//3번 두더지 올라오는 시간간격

unsigned long t_lim_prev=0;//제한시간 관련 변수

unsigned int time_limit=31000;//남은시간 31초로 설정
unsigned int kill_dudergi=0;//잡은 두더지 개수

int up_count1=0;//1번 두더지 올라왔는지 판단하는 변수
int up_count2=0;//2번 두더지 올라왔는지 판단하는 변수
int up_count3=0;//3번 두더지 올라왔는지 판단하는 변수
int game_start_flag=0;//게임시작 조건 플래그비트
int interrupt_speed_flag=1;//속도조절 플래그 비트

/* 레이저의 불안정으로 인해 인터럽트가 계속 발생되어 카운트가 재대로 올라가지 않는 문제가 발생
   이를 해결하기 위해 중복방지 플래그 비트가 if 1이면 더이상 카운트하지 않도록 코딩함 */

int dudergi1_duplication_Prevention_flag=0;//두더지1 카운트 중복방지를 위한 플래그비트
int dudergi2_duplication_Prevention_flag=0;//두더지2 카운트 중복방지를 위한 플래그비트
int dudergi3_duplication_Prevention_flag=0;//두더지3 카운트 중복방지를 위한 플래그비트


/*세븐세그먼트에서 표시할 숫자배열*/ 
const unsigned int num[10][7]={
  {1,1,1,1,1,1,0},//0
  {0,1,1,0,0,0,0},//1
  {1,1,0,1,1,0,1},//2
  {1,1,1,1,0,0,1},//3
  {0,1,1,0,0,1,1},//4
  {1,0,1,1,0,1,1},//5
  {1,0,1,1,1,1,1},//6
  {1,1,1,0,0,1,0},//7
  {1,1,1,1,1,1,1},//8
  {1,1,1,1,0,1,1},//9
};


/*세븐세그먼트 끄는 함수*/
void display_claer(){
  for(int i=0; i<7; i++){
    digitalWrite(led[i],0);
  }

  for(int i=1; i<=4; i++){
    digitalWrite(com[i],1);
  }
}


/*세븐세그먼트 표시함수(나타낼수,표현할 자리)*/
void display_number(unsigned int n,unsigned int m){//n:나태내고 싶은 수, m:표현할 자리 
  display_claer();
  for(int i=0; i<7; i++){
    digitalWrite(led[i],num[n][i]);
  }
  digitalWrite(com[m],0);
}


/*잡은 두더지수 세븐세그먼트에 표시하는 함수*/
void display_kill_dudergi(unsigned int kill_dudergi){
  
  int kill_dudergi_10=kill_dudergi/10; //잡은 마리수의 10의 자리
  int kill_dudergi_1=kill_dudergi%10; //잡은 마리수의 1의자리

  display_number(kill_dudergi_10,3);
  display_number(kill_dudergi_1,4);
}


/*남은시간 세븐세그먼트에 표시하는 함수*/
void display_time_limit(int time_limit){
  int time_limit_10=time_limit/10000; //남은 시간의 10의 자리 초
  int time_limit_1=(time_limit%10000)/1000; //남은 시간의 1의 자리 초

  display_number(time_limit_10,1);
  display_number(time_limit_1,2);
}


/*인터럽트 버튼 누를때 마다 올라오는 속도 조절하는 함수*/
void button_interrupt_time_control(){
  t1_delay=t1_delay+300;
  t2_delay=t2_delay+300;
  t3_delay=t3_delay+300;
  if(t1_delay>1600) t1_delay=1000;
  if(t2_delay>1850) t2_delay=1250;
  if(t3_delay>2100) t3_delay=1500;
}


/*인터럽트를 활용해 레이저를 감지하고 잡은두더지1,2,3을 카운트 하는함수*/
/*두더지가 올라와 있는 시간(500ms)동안만 up_count플래그 비트를 1로하여 카운트 할 수 있도록함*/
void raser_recev_interrupt1(){//인터럽트가 발생하면
  if(up_count1){//두더지가 올라와 있을때
    if(!dudergi1_duplication_Prevention_flag){//중첩방지 플래그 비트가 활성화 되있지 않을때
      kill_dudergi+=1;//두더지 카운트
      dudergi1_duplication_Prevention_flag=1;//더이상 두더지 카운트가 올라가지 않도록 중복방지 플래그비트 활성화
    }
  }
}

/*위와 동일*/
void raser_recev_interrupt2(){
  if(up_count2){
    if(!dudergi2_duplication_Prevention_flag){
      kill_dudergi+=1;
      dudergi2_duplication_Prevention_flag=1;
    }
  }
}

/*위와 동일*/
void raser_recev_interrupt3(){
  if(up_count3){
    if(!dudergi3_duplication_Prevention_flag){
      kill_dudergi+=1;
      dudergi3_duplication_Prevention_flag=1;
    }
  }
}

/*두더지 1번동작에 필요한 함수, 클래스를 통해 구현*/
void func1_1(){
  dudergi_1.write(90);
}
void func1_2(){
  dudergi_1.write(0);
}

/*두더지 2번동작에 필요한 함수, 클래스를 통해 구현*/
void func2_1(){
  dudergi_2.write(90);
}
void func2_2(){
  dudergi_2.write(0);
}

/*두더지 3번동작에 필요한 함수, 클래스를 통해 구현*/
void func3_1(){
  dudergi_3.write(90);
}
void func3_2(){
  dudergi_3.write(0);
}

NDelayFunc dudergi__1(func1_1,func1_2);//두더지1 동작함수를 가지는 두더지1 객체 선언
NDelayFunc dudergi__2(func2_1,func2_2);//두더지2 동작함수를 가지는 두더지2 객체 선언
NDelayFunc dudergi__3(func3_1,func3_2);//두더지3 동작함수를 가지는 두더지3 객체 선언


void setup() {

  /*4자리 7세그먼트 초기화*/
  for(int i=0; i<7; i++){
    pinMode(led[i],OUTPUT);
  }

  for(int i=1; i<=4; i++){
    pinMode(com[i],OUTPUT);
  }
  
  /*입력,출력에 필요한 핀모드 설정*/
  pinMode(interrupt_button_switch,INPUT);
  pinMode(raser_send_pin,OUTPUT);
  pinMode(reaser_recev_pin1,INPUT);
  pinMode(reaser_recev_pin2,INPUT);
  pinMode(reaser_recev_pin3,INPUT);
  
  /*서보모터 초기화*/
  dudergi_1.attach(9);
  dudergi_2.attach(10);
  dudergi_3.attach(11);

  dudergi_1.write(0);
  dudergi_2.write(0);
  dudergi_3.write(0);


  /* 게임시작 버튼을 누르기전엔 레이저를 끔*/
  digitalWrite(raser_send_pin,0);

  /* 인터럽트 함수 */
  attachPCINT(digitalPinToPCINT(reaser_recev_pin1),raser_recev_interrupt1,FALLING);
  attachPCINT(digitalPinToPCINT(reaser_recev_pin2),raser_recev_interrupt2,FALLING);
  attachPCINT(digitalPinToPCINT(reaser_recev_pin3),raser_recev_interrupt3,FALLING);
  attachPCINT(digitalPinToPCINT(interrupt_button_switch),button_interrupt_time_control,RISING);

}

void loop() {
  
  if(digitalRead(interrupt_button_switch)==1){//사용자가 게임시작 버튼을 누르면
    game_start_flag=1;//게임시작 플래그 비트를 1로 활성화
  }

  if(game_start_flag){//게임시작 플래그비트가 활성화 되있을때만 두더지잡기 게임이 작동


  digitalWrite(raser_send_pin,1);//게임이 시작되면 레이저를 킴
  unsigned long t_lim_now=millis();//현재 시간 측정 millis()함수
  
  dudergi__1.run(t1_delay,&up_count1,&dudergi1_duplication_Prevention_flag);//두더지1 동작문(올라오는주기,카운트 플래그비트,중복방지 플래그비트), 카운트 및 중복방지 플래그비트는 cpp파일과 값을 공유하기 위해 주소를 전달  
  dudergi__2.run(t2_delay,&up_count2,&dudergi2_duplication_Prevention_flag);//두더지2 동작문(올라오는주기,카운트 플래그비트,중복방지 플래그비트), 카운트 및 중복방지 플래그비트는 cpp파일과 값을 공유하기 위해 주소를 전달 
  dudergi__3.run(t3_delay,&up_count3,&dudergi3_duplication_Prevention_flag);//두더지3 동작문(올라오는주기,카운트 플래그비트,중복방지 플래그비트), 카운트 및 중복방지 플래그비트는 cpp파일과 값을 공유하기 위해 주소를 전달 



  /*남은시간 카운트*/
  if(t_lim_now-t_lim_prev>=1000){//1초에 한번씩
    t_lim_prev=t_lim_now;
    time_limit=time_limit-1000;//시간 카운트 다운
  }
  
  display_time_limit(time_limit);//남은시간 표시
  display_kill_dudergi(kill_dudergi);//잡은 두더지수 표시

  /*제한시간이 다 되었을때 필요한 설정*/
  if(time_limit==0){
    game_start_flag=0;//동작 멈추기
    digitalWrite(raser_send_pin,0);//레이저 끄기
    time_limit=30000;//제한시간 초기화
    kill_dudergi=0;//잡은 두더지 초기화

    //두더지 올라오는 주기 초기화
    t1_delay=700;
    t2_delay=950;
    t3_delay=1200;

    //두더지 위치 초기화
    dudergi_1.write(0);
    dudergi_2.write(0);
    dudergi_3.write(0);
    
    display_claer();
  }
  }
}


