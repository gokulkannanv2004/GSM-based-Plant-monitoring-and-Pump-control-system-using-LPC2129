#ifndef __UART_HEADER_H__

#define __UART_HEADER_H__


#include <lpc21xx.h>


void UART0_CONFIG(void);

void UART0_TX(unsigned char);

unsigned char UART0_RX(void);

void UART0_STR(  char *);


void UART0_CONFIG(void)

{

    PINSEL0 = 0x05;


    U0LCR = 0x83;

    U0DLL = 97;

    U0DLM = 0;

    U0LCR = 0x03;

}


void UART0_TX(unsigned char d)

{

    while((U0LSR & (1 << 5)) == 0);


    U0THR = d;

}


unsigned char UART0_RX(void)

{

    while((U0LSR & (1 << 0)) == 0);


    return U0RBR;

}


void UART0_STR( char *s)

{

    while(*s)

    {

        UART0_TX(*s++);

    }

}


#endif


