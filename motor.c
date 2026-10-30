#include <lpc21xx.h>

#include "Motor.h"


void MOTOR_ON(void)

{

    IOSET0 = MOTOR_IN;

    IOCLR0 = MOTOR_OUT;

}


void MOTOR_OFF(void)

{

    IOCLR0 = MOTOR_IN;

    IOCLR0 = MOTOR_OUT;

}
