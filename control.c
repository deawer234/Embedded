/*
 * control.c
 *
 *  Created on: 11. 5. 2022
 *      Author: denyh
 */

#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "digiswitch.h"
#include "queue.h"
#include "lcd.h"
#include "file.h"
#include "key.h"
#include "green_led.h"

#define WR_POS 0x86
#define BUTTON_POS 0xC4
#define ESC 0x1B


enum CM_states
{
   CM_READY,
   CM_PAYMENT,
   CM_CHANGE,
};


void control_task(void* pvParameters){
    INT8U state = CM_READY;
    INT8U option = 0;
    INT8U req = 0;
    INT8U cash = 0;
    char str[20];
    while(1){
        switch(state){
            case CM_READY: // Ready state, waiting for selection of caffe
                if(get_keyboard(&option) == pdTRUE){
                    switch(option - '0'){
                        case 1:// Esspreso
                            req = 16;
                            wr_ch_LCD(ESC);
                            wr_str_LCD("@Selected Espresso");
                            state = CM_PAYMENT;
                            break;
                        case 2:// Latte
                            req = 26;
                            wr_ch_LCD(ESC);
                            wr_str_LCD("@Selected Latte");
                            state = CM_PAYMENT;
                            break;
                        case 3:// Filter
                            break;
                    }
                    vTaskDelay(500);
                    wr_ch_LCD(ESC);
                    wr_str_LCD("@Insert cash");
                }
                break;
            case CM_PAYMENT: //Waiting for insert of cash
                if(get_re(&option) == pdTRUE){
                    switch(option - '0'){
                        case 1:
                            cash += 20;
                            break;
                        case 2:
                            cash += 5;
                            break;
                    }
                    sprintf(str, "%d", cash);
                    wr_ch_LCD(ESC);
                    wr_str_LCD("@CASH: ");
                    wr_str_LCD(str);
                }
                break;
            case CM_CHANGE: // Giving back change to user
                set_change(cash - req);
                break;

        }
    }
}

