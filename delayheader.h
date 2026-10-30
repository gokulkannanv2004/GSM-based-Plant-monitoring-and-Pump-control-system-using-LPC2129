#ifndef __DELAY_HEADER_H__

#define __DELAY_HEADER_H__


void delay_seconds(unsigned int seconds)

{

    T0PR = 15000000 - 1;

    T0TCR = 0x01;


    while(T0TC < seconds);


    T0TCR = 0x03;

    T0TCR = 0x00;

}


void delay_milliseconds(unsigned int milliseconds)

{

    T0PR = 15000 - 1;

    T0TCR = 0x01;


    while(T0TC < milliseconds);


    T0TCR = 0x03;

    T0TCR = 0x00;

}


#endif
