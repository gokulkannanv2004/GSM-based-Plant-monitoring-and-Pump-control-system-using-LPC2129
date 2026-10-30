#include <lpc21xx.h>

#include <string.h>


#include "delayheader.h"

#include "uart_header.h"

#include "gsm_header.h"

#include "lcdheader.h"

#include "Motor.h"

#include "Motor.c"


#define SOIL_SENSOR  2


char received_msg[100];


int soil_sensor_read(void)

{

    if((IOPIN0 &(1U<<SOIL_SENSOR)) == 0)

        return 0;       // wet

    else

        return 1;       // dry

}


int main()

{

    int soil_status;


    /* Soil sensor as input */

    IODIR0 &= ~(1U << SOIL_SENSOR);


    /* Motor pins as output */

    IODIR0 |= MOTOR_IN | MOTOR_OUT;


    MOTOR_OFF();


    /* UART initialization */

    UART0_CONFIG();


    /* GSM initialization */

    GSM_CONFIG();


    /* LCD initialization */

    LCD_INITIALIZATION();


    LCD_COMMAND(0x01);

    LCD_COMMAND(0x80);

    LCD_STR("SMART IRRIGATION");


    delay_milliseconds(1000);


    while(1)

    {

        /*

           Wait for SMS from user.

           SMS format expected:

           *status#

        */


        GSM_RECEIVE_SMS(received_msg);


        LCD_COMMAND(0x01);

        LCD_COMMAND(0x80);

        LCD_STR("MSG RECEIVED");


        delay_milliseconds(1000);


       

        if(strcmp(received_msg, "status") == 0)

        {

            soil_status = soil_sensor_read();


            if(soil_status==1)

            {

                /* Soil is dry */


                LCD_COMMAND(0x01);

                LCD_COMMAND(0x80);

                LCD_STR("SOIL IS DRY");


                GSM_SEND_MSG("DRY");
			 }
			 else
			 {

				LCD_COMMAND(0x01);

                LCD_COMMAND(0x80);

                LCD_STR("SOIL IS WET");


                GSM_SEND_MSG("WET");
             }
			 } 
		else if(strcmp(received_msg,"pump on")==0)
		{
		MOTOR_ON();
		 	LCD_COMMAND(0x01);

            LCD_COMMAND(0x80);

            LCD_STR("PUMP ON");
            GSM_SEND_MSG("PUMP ON");
			delay_seconds(60);
			MOTOR_OFF();
			LCD_COMMAND(0x01);

            LCD_COMMAND(0x80);

            LCD_STR("PUMP OFF");
            GSM_SEND_MSG("PUMP OFF");
                
		 }
	 }
  }      

		      