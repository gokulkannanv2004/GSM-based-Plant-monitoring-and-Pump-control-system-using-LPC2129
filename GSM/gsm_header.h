#ifndef __GSM_HEADER_H__

#define __GSM_HEADER_H__


#include <lpc21xx.h>

#include "delayheader.h"

#include "uart_header.h"


#define USER_NUMBER "+916238149432"


void GSM_CONFIG(void);

void GSM_SEND_MSG(char *);

void GSM_RECEIVE_SMS(char *);


void GSM_CONFIG(void)

{

    delay_milliseconds(2000);


    UART0_STR("AT\r\n");

    delay_milliseconds(1000);	  //100
	//UART0_STR("ATE0\r\n");
	//	delay_milliseconds(500);

    UART0_STR("AT+CMGF=1\r\n");

    delay_milliseconds(500);	//100


    UART0_STR("AT+CNMI=2,2,0,0,0\r\n");

    delay_milliseconds(500);	//100

}


void GSM_SEND_MSG(char *msg)

{

    UART0_STR("AT+CMGS=\"");

    UART0_STR(USER_NUMBER);

    UART0_STR("\"\r\n");


    delay_milliseconds(2000);


    UART0_STR(msg);


    UART0_TX(0x1A);


    delay_milliseconds(5000);

}


void GSM_RECEIVE_SMS(char *buffer)

{

 unsigned char ch;

    unsigned int i = 0;


    do

    {

        ch = UART0_RX();

    }while(ch != '*');

	while(1)
	{
	ch =UART0_RX();
	if(ch=='#')
	     break;
	if(i<99)
	{
	buffer[i++]=ch;
	}
	buffer[i]='\0';

	 
      }  


   }



#endif

