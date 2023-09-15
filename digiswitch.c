/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: digiswitch.c
*
* PROJECT....: ECP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 220424  DN    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include <math.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "emp_type.h"
#include "lcd.h"
#include "event_groups.h"

/*****************************    Defines    *******************************/
#define CW      0x20
#define CCW     0x40
#define NO_EVENT 0
#define CW_BIT   1
#define CCW_BIT   2
#define BUTTON  0x80
#define NOT_TRIG  0x00
#define A_TRIG  0x02
#define B_TRIG  0x01

enum states{
    READING,
    RELEASE,
    STEP,
    FIRE_EVENT
};
/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
//EventGroupHandle_t digiswitch_event;
QueueHandle_t Q_RE;

/*****************************   Functions   *******************************/
void digiswitch_init(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :  Init
*****************************************************************************/
{
    GPIO_PORTA_DEN_R = 0xE0;

    GPIO_PORTA_PUR_R = 0x80;

    Q_RE = xQueueCreate(32, sizeof(INT8U));
    //digiswitch_event = xEventGroupCreate();

}

BaseType_t get_re(INT8U* ch){
    return( xQueueReceive(Q_RE, ch, portTICK_RATE_MS ));
}

void digiswitch_task(void *pvParameters)
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Driver for rotary encoder
******************************************************************************/
{
    INT8S state = READING;
    INT8S event = 0;
    INT8S a = 0, b = 0, a1 = 0, b1 = 0, y = 0;

    while(1){

        switch(state){
        case READING:
            if(GPIO_PORTA_DATA_R & CW){
                a = NOT_TRIG;
            }else{
                a = A_TRIG;
            }
            if(GPIO_PORTA_DATA_R & CCW){
                b = NOT_TRIG;
            }else{
                b = B_TRIG;
            }
            state = STEP;
            break;
        case STEP:
            if((a | b) == (a1 | b1)){
                state = READING;
                break;
            }
            y = ((a | b)^(a1 | b1));
            if((a == b)||(a & A_TRIG && b & B_TRIG)){
                if(y == B_TRIG){
                    event = CW_BIT;
                    state = FIRE_EVENT;
                }
                else{
                    if(y == A_TRIG){
                        event = CCW_BIT;
                        state = FIRE_EVENT;
                    }
                }
            }
            else{
                if(y == A_TRIG){
                    event = CW_BIT;
                    state = FIRE_EVENT;
                }
                else{
                    if(y == B_TRIG){
                        event = CCW_BIT;
                        state = FIRE_EVENT;
                    }
                }
            }
            a1 = a;
            b1 = b;
            break;
        case FIRE_EVENT:
            xQueueSend(Q_RE, &event, portTICK_RATE_MS);
            //xEventGroupSetBits(digiswitch_event, event);
            //event = NO_EVENT;
            event = 0;
            vTaskDelay(3);
            state = READING;
            break;
        }
     }
}

/****************************** End Of Module *******************************/





