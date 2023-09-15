

/**
 * main.c
 */

#include <stdint.h>
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
#include "control.h"
#include "status_led.h"
#include "green_led.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3


static void setupHardware(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
  // TODO: Put hardware configuration and initialisation in here

  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
  int dummy;
  SYSCTL_RCGC2_R  =  SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOE | SYSCTL_RCGC2_GPIOF;
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;
  // Do a dummy read to insert a few cycles after enabling the peripheral.
  dummy = SYSCTL_RCGC2_R;




  GPIO_PORTA_DIR_R = 0x1C;
  GPIO_PORTC_DIR_R = 0xF0;
  GPIO_PORTD_DIR_R = 0x4C;
  GPIO_PORTF_DIR_R = 0x0E;

  GPIO_PORTC_DEN_R = 0xF0;
  GPIO_PORTA_DEN_R = 0x7C;
  GPIO_PORTD_DEN_R = 0x4C;
  GPIO_PORTE_DEN_R = 0x0F;
  GPIO_PORTF_DEN_R = 0x1F;

  //GPIO_PORTF_LOCK_R = 0x4C4F434B;        // 2) unlock GPIO Port F
  //GPIO_PORTF_CR_R = 0x1F;              // allow changes to PF4-0


  GPIO_PORTF_PUR_R = 0x11;
  GPIO_PORTA_PUR_R = 0x80;


  init_systick();
  digiswitch_init();
  key_init();
}

int main(void)
{
    setupHardware();
    //xTaskCreate( scale_task, "Scale",  USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    //xTaskCreate( ui_task,  "UI",  USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( status_led_task, "status_LED", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( green_led_task, "green_LED", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( digiswitch_task, "Digiswitch", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( key_task, "keyboard", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( lcd_task, "LCD", USERTASK_STACK_SIZE, NULL, MED_PRIO , NULL );
    xTaskCreate( control_task, "control", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL );
    vTaskStartScheduler();
	return 0;
}
