#ifndef _TODOS_
#define _TODOS_
#define TC0 0
#define TC1 0
#define ID_TC1 0
#define ID_TC4 0

#define TC_CMR_WAVSEL_UP_RC 0
#define TC_CMR_WAVE 0
#define TC_CMR_WAVSEL_UP 0
#define TC_CMR_TCCLKS_TIMER_CLOCK2 0

#include <inttypes.h>

void pmc_enable_periph_clk(int x){
}
void TimeTick_Configure(int x){
}
void pmc_set_writeprotect(bool val){
}
void NVIC_SetPriorityGrouping(int x){
}
int NVIC_EncodePriority(int a, int b,int c){
	return 1;
}
void TC_FindMckDivisor(int a, int b,uint32_t* tc_count,uint32_t* tc_clock,int n){
}
void TC_Configure(int a, int b, uint32_t c){
}
void TC_SetRC(int a, int b, uint32_t c){}
void TC_Start(int a, int b){}
void __set_BASEPRI(int a){}
#endif
