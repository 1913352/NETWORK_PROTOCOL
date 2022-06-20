#include "mbed.h"           //메시지 만들어 주는 해석하는 역할
#include "L3_msg.h"
//각 종류마다 필요한 메시지 정의 
//함수 짜고 main에 함수 호출

//PDU type 판정 함수
int L3_Msg_checkIfData(uint8_t* msg)
{
    return (msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_DATA);
}

int L3_Msg_checkIfReq(uint8_t* msg)
{
    return (msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_REQ);
}

int L3_Msg_checkIfBrd(uint8_t* msg)
{
    return (msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_BRD);
}

int L3_Msg_checkIfBrdNei(uint8_t* msg)
{
    return (msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_BRDNEI);
}

int L3_Msg_checkIfREP(uint8_t* msg)
{
    return (msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_REP);
}


//PDU 생성 함수      몰겠음 ㅠ
uint8_t L3_Msg_encodeReq(uint8_t* msg_ack, uint8_t seq);
{
    msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_REQ);
    return 
}

uint8_t L3_Msg_encodeData(uint8_t* msg_data, uint8_t* data, int );
{
    msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_DATA);
    return 
}


//PDU 내용 추출 함수
uint8_t L3_Msg_getSeq(uint8_t* msg)     
{
    return msg[L3_MSG_OFFSET_SEQ];
}

uint8_t* L3_Msg_getWord(uint8_t* msg)   
{
    return &msg[L3_MSG_OFFSET_DATA];
}
