#include "Arduino.h"
#include "NDelayFunc.h"

/*두더지 객체 생성자*/
NDelayFunc::NDelayFunc(
    void(*func1)(),
    void(*func2)()):
    t_prev(0),func1(func1),func2(func2){}//t_prev를 0으로 받고 함수1,2를 인수로 받아서 초기화함 


/*두더지 동작문*/
/*main.c에 있는 전역변수와 값을 공유해야 하기 때문에 변수의 주소를 인수로 받아 *참조 연산자로 값을 변경함  */
void NDelayFunc::run(int dudergi_delay,int* up_count,int* dudergi_duplication_Prevention_flag){
    unsigned long t_now=millis();

    if(t_now-t_prev>=dudergi_delay){//올라오는 주기가 되면
        t_prev=t_now;

        func1();//dudergi.write(90), 두더지가 올라옴
    }

    if(t_prev<=t_now && t_now<=t_prev+500){//두더지가 올라와 있을때(500ms간)
    *up_count=1;//카운트 플래그 비트 활성화
    }
    else{//내려가 있다면 
    *up_count=0;//카운트 플래그비트 비활성화
    *dudergi_duplication_Prevention_flag=0;//두더지 중복 방지 플래그비트 비활성화
    }
    
    if(t_now-t_prev>=500){//두더지가 올라온후 500ms후
        func2();//dudergi.write(0), 두더지가 내려감 
    }


}
