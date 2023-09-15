/*
 * green_led.c
 *
 *  Created on: 11. 5. 2022
 *      Author: denyh
 */
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "emp_type.h"

QueueHandle_t Q_GREEN;

void green_led_init(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
  INT8S dummy;
  // Enable the GPIO port that is used for the on-board LED.
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

  // Do a dummy read to insert a few cycles after enabling the peripheral.
  dummy = SYSCTL_RCGC2_R;

  GPIO_PORTF_DIR_R |= 0x08;
  GPIO_PORTF_DEN_R |= 0x08;
}

void set_change(INT8U change){
    return (xQueueSend(Q_GREEN, &change, portTICK_RATE_MS));
}

void green_led_task(void *pvParameters)
{
  green_led_init();
  INT8U change = 0;
  Q_GREEN = xQueueCreate(4, sizeof(INT8U));
  while(1)
  {
    // Toggle green led
    while(xQueueReceive(Q_GREEN, &change, portTICK_RATE_MS) != pdTRUE && change == 0);

    GPIO_PORTF_DATA_R ^= 0x08;
    change--;
    vTaskDelay( 500 / portTICK_RATE_MS); // wait 1500 ms.
  }
}

