#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"


//FSM state -------------------------------------------------
#define L3STATE_IDLE                0       //위치등록 안된 상태에서 가만히 기다리는 state
#define L3STATE_Wait_REP            1       //기지국에게 위치등록 reply 회신올 때까지 기다리는 state
#define L3STATE_Camping             2       //위치등록 된 상태에서 가만히 기다리는 state
#define L3STATE_Handover            3       //위치등록 된 상태에서 reply 기다림


//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//SDU (input)
static uint8_t originalWord[200];
static uint8_t wordLen=0;

static uint8_t sdu[200];

// //PDU
// static uint8_t req_msg;
// static uint8_t 
// //static uint8_t pduSize;


//serial port interface
static Serial pc(USBTX, USBRX);


//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (!L3_event_checkEventFlag(L3_event_dataToSend))
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            L3_event_setEventFlag(L3_event_dataToSend);
            debug_if(DBGMSG_L3,"word is ready! ::: %s\n", originalWord);
        }
        else
        {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_dataToSend);
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);
            }
        }
    }
}



void L3_initFSM()
{
    //initialize service layer
    pc.attach(&L3service_processInputWord, Serial::RxIrq);

    pc.printf("Give a word to send : ");
}

void L3_FSMrun(void)
{   
    if (prev_state != main_state)
    {
        debug_if(DBGMSG_L3, "[L3] State transition from %i to %i\n", prev_state, main_state);
        prev_state = main_state;
    }

    //FSM should be implemented here! ---->>>>
    switch (main_state)
    {
        case L3STATE_IDLE: //IDLE state description
            
            if (L3_event_checkEventFlag(L3_event_msgRcvd)) //if data reception event happens
            {
                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t* getWordData = L3_Msg_getWord(dataPtr);
                uint8_t size = L3_LLI_getSize();

                //event A-1: Brd msg 수신 -> wait reply state 이동, request msg 송신  TX timer start           
                if (L3_Msg_checkIfBrd(dataPtr)) 
                {   
                    
                    //Req msg 송신(PDU)
                    strcpy((char*)sdu), (char*)originalWord;
                    L3_LLI_dataReqFunc(sdu, wordLen);

                    L3_timer_TX_startTimer();

                    main_state = L3STATE_Wait_REP;
                    wordLen = 0;

                }

            }
            else{
                wordLen = 0;
                L3_timer_TX_stopTimer();
                L3_event_clearEventFlag(L3_event_dataToSend);
                main_state = L3STATE_IDLE;
            }

        break;
        

        case L3STATE_Wait_REP:

            if (L3_event_checkEventFlag(L3_event_msgRcvd))
            {
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t* getWordData = L3_Msg_getWord(dataPtr);
                uint8_t size = L3_LLI_getSize();

                //event A: Brd msg 수신

                if(L3_Msg_checkIfBrd(dataPtr))
                {

                        main_state = L3STATE_IDLE;
                    }
                


                //event B: rep msg 수신     ->  위치등록 메시지 출력    TX timer stop, Brd timer start
                else if(L3_Msg_checkIfRep(dataPtr))
                {

                    pc.printf("\n -----------------------------------\n Now the device is near %s base station! \n -----------------------------------\n", getWordData)
                    L3_timer_TX_stopTimer();
                    L3_timer_Brd_startTimer();


                    main_state = L3STATE_Camping;
                }


                //event C: Tx timer timeout    
                else if (L3_event_checkEventFlag(L3_event_TXTimeout))
                {
                    pc.printf("\n -----------------------------------\n TX time is over \n -----------------------------------\n")
                    L3_event_clearEventFlag(L3_event_TXTimeout);
                    main_state = L3STATE_IDLE;
                }

            }


        break;

        case L3STATE_Camping:
            if (L3_event_checkEventFlag(L3_event_msgRcvd))
            {   
                //event A-1     --> Brd timer start
                if(L3_Msg_checkIfBrd(dataPtr))
                    
                    
                    L3_timer_Brd_startTimer();

                    main_state = L3STATE_Camping;
                }

                //event A-2
                else if(L3_Msg_checkIfBrdNEI(dataPtr))
                {
                    //if (신호 약하면)  ->  Camping State 머무름
                    main_state = L3STATE_Camping;


                    //if (신호 강하면)  ->  request msg 송신, TX timer 시작, Handover State 이동
                    //req msg 송신
                    strcpy((char*)sdu), (char*)originalWord;
                    L3_LLI_dataReqFunc(sdu, wordLen);

                    L3_timer_TX_startTimer();
                    main_state = L3STATE_Handover;
                }

                //event D: Brd timer time out   ->  IDLE State 이동
                else if (L3_event_checkEventFlag(L3_event_BrdTimeout))
                {
                    pc.printf("\n -----------------------------------\n Brd time is over \n -----------------------------------\n")
                    L3_event_clearEventFlag(L3_event_BrdTimeout);
                    main_state = L3STATE_IDLE;
                }
            
        break;

        case L3STATE_Handover:
            //event C: TX timer time out    ->  camping state로 이동
            if (L3_event_checkEventFlag(L3_event_TXTimeout))
            {
                pc.printf("\n -----------------------------------\n TX time is over \n -----------------------------------\n");
                L3_event_clearEventFlag(L3_event_TXTimeout);
                main_state = L3STATE_Camping;
            }
            
            //event B: reply msg 수신   ->  serving cell 변경, TX timer stop, Brd timer start, Camping State로 이동
            else if (L3_Msg_checkIfRep(dataPtr))
            {
                //serving cell 변경 -> 출력
                pc.printf("\n -----------------------------------\n Now the device is near %s base station! \n -----------------------------------\n", getWordData);
                //어떻게 변경하냐...

                L3_timer_TX_stopTimer();
                L3_timer_Brd_startTimer();
                main_state = L3STATE_Camping;
            }

        break;
