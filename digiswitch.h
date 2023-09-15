/*
 * digiswitch.h
 *
 *  Created on: 23. 4. 2022
 *      Author: denyh
 */
#include "freeRTOS.h"
//#include "event_groups.h"
#include "emp_type.h"

#ifndef DIGISWITCH_H_
    #define DIGISWITCH_H_


void digiswitch_init(void);
BaseType_t get_re(INT8U *ch);
//EventGroupHandle_t get_event(void);
void digiswitch_task(void *pvParameters);

#endif /* DIGISWITCH_H_ */
