#include "mbed.h"
#include "L3_FSMevent.h"
#include "protocol_parameters.h"


//ARQ retransmission timer
static Timeout TX_timer;   
static Timeout Brd_timer;                    
static uint8_t TX_timerStatus = 0;
static uint8_t Brd_timerStatus = 0;


//timer event : ARQ timeout
void L3_timer_TX_timeoutHandler(void) 
{
    TX_timerStatus = 0;
    L3_event_setEventFlag(L3_event_TXTimeout);
}

void L3_timer_Brd_timeoutHandler(void) 
{
    Brd_timerStatus = 0;
    L3_event_setEventFlag(L3_event_BrdTimeout);
}

//timer related functions ---------------------------

//TX timer
void L3_timer_TX_startTimer()
{
    uint8_t waitTime = 1;//L2_ARQ_MINWAITTIME + rand()%(L2_ARQ_MAXWAITTIME-L2_ARQ_MINWAITTIME); //timer length
    TX_timer.attach(L3_timer_TX_timeoutHandler, waitTime);
    TX_timerStatus = 1;
}

void L3_timer_TX_stopTimer()
{
    TX_timer.detach();
    TX_timerStatus = 0;
}

uint8_t L3_timer_TX_getTimerStatus()
{
    return TX_timerStatus;
}

//Brd timer
void L3_timer_Brd_startTimer()
{
    uint8_t waitTime = 1;//L2_ARQ_MINWAITTIME + rand()%(L2_ARQ_MAXWAITTIME-L2_ARQ_MINWAITTIME); //timer length
    Brd_timer.attach(L3_timer_Brd_timeoutHandler, waitTime);
    Brd_timerStatus = 1;
}

void L3_timer_Brd_stopTimer()
{
    Brd_timer.detach();
    Brd_timerStatus = 0;
}

uint8_t L3_timer_Brd_getTimerStatus()
{
    return Brd_timerStatus;
}