#ifndef NDelayFunc_h_
#define NDelayFunc_h_

/*
extern int up_count1;//1번 두더지 올라왔는지 판단하는 변수
extern int up_count2;//2번 두더지 올라왔는지 판단하는 변수
extern int up_count3;//3번 두더지 올라왔는지 판단하는 변수

extern int dudergi1_duplication_Prevention_flag;//두더지1 카운트 중복방지를 위한 플래그비트
extern int dudergi1_duplication_Prevention_flag;//두더지2 카운트 중복방지를 위한 플래그비트
extern int dudergi1_duplication_Prevention_flag;//두더지3 카운트 중복방지를 위한 플래그비트
*/
//extern int interrupt_speed_flag;

class NDelayFunc{
    unsigned long t_prev;
    void(*func1)();
    void(*func2)();
    

    public:
    NDelayFunc(
        void(*func1)(),
        void(*func2)());
    void run(int dudergi_delay,int* up_count,int* dudergi_duplication_Prevention_flag);
};

#endif