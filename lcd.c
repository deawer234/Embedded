/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.c
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
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "emp_type.h"
#include "lcd.h"

/*****************************    Defines    *******************************/
#define lcdSHORT_DELAY 3
#define QUEUE_LEN   128

enum LCD_states
{
  LCD_READY,
  LCD_ESC_RECEIVED,
};

/*****************************   Constants   *******************************/
const INT8U LCD_init_sequense[]=
{
  0x30,		// Reset
  0x30,		// Reset
  0x30,		// Reset
  0x20,		// Set 4bit interface
  0x28,		// 2 lines Display
  0x0C,		// Display ON, Cursor OFF, Blink OFF
  0x06,		// Cursor Increment
  0x01,		// Clear Display
  0x02,     	// Home
  0xFF		// stop
};

/*****************************   Variables   *******************************/
//INT8U LCD_buf[QUEUE_LEN];
//INT8U LCD_buf_head = 0;
//INT8U LCD_buf_tail = 0;
//INT8U LCD_buf_len  = 0;

INT8U LCD_init;
static QueueHandle_t Q_LCD;

/*****************************   Functions   *******************************/
INT8U wr_ch_LCD( INT8U Ch )
/*****************************************************************************
*   OBSERVE  : LCD_PROC NEEDS 20 mS TO PRINT OUT ONE CHARACTER
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  return( xQueueSend( Q_LCD, &Ch, portTICK_RATE_MS ));
}

void wr_str_LCD( char *pStr )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
    while( *pStr )
    {
      wr_ch_LCD( *pStr );
      pStr++;
    }

}

void move_LCD( INT8U x, INT8U y )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  INT8U Pos;

  Pos = y*0x40 + x;
  Pos |= 0x80;
  wr_ch_LCD( ESC );
  wr_ch_LCD( Pos );
}
//----------------------------

void wr_ctrl_LCD_low( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write low part of control data to LCD.
******************************************************************************/
{
  INT8U temp;
  volatile int i;

  temp = GPIO_PORTC_DATA_R & 0x0F;
  temp  = temp | ((Ch & 0x0F) << 4);
  GPIO_PORTC_DATA_R  = temp;
  for( i=0; i<1000; i )
      i++;
  GPIO_PORTD_DATA_R &= 0xFB;        // Select Control mode, write
  for( i=0; i<1000; i )
      i++;
  GPIO_PORTD_DATA_R |= 0x08;        // Set E High

  for( i=0; i<1000; i )
      i++;

  GPIO_PORTD_DATA_R &= 0xF7;        // Set E Low

  for( i=0; i<1000; i )
      i++;
}

void wr_ctrl_LCD_high( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write high part of control data to LCD.
******************************************************************************/
{
  wr_ctrl_LCD_low(( Ch & 0xF0 ) >> 4 );
}

void wr_ctrl_LCD( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write control data to LCD.
******************************************************************************/
{
  static INT8U Mode4bit = 0;
  INT16U i;

  wr_ctrl_LCD_high( Ch );
  if( Mode4bit )
  {
    for(i=0; i<1000; i++);
    wr_ctrl_LCD_low( Ch );
  }
  else
  {
    if( (Ch & 0x30) == 0x20 )
      Mode4bit = 1;
  }
}


void lcd_init(void){
    /*****************************************************************************
    *   Input    : -
    *   Output   : -
    *   Function : Init.
    ******************************************************************************/
    for(;;){
        if(LCD_init_sequense[LCD_init] != 0xFF){
            wr_ctrl_LCD(LCD_init_sequense[LCD_init++]);
            vTaskDelay(lcdSHORT_DELAY);
        }else{
            break;
        }
    }
}


void out_LCD_low( INT8U Ch )
/*****************************************************************************
*   Input    : Mask
*   Output   : -
*   Function : Send low part of character to LCD.
*              This function works only in 4 bit data mode.
******************************************************************************/
{
  INT8U temp;

  temp = GPIO_PORTC_DATA_R & 0x0F;
  GPIO_PORTC_DATA_R  = temp | ((Ch & 0x0F) << 4);
  GPIO_PORTD_DATA_R &= 0x7F;        // Select write
  GPIO_PORTD_DATA_R |= 0x04;        // Select data mode
  GPIO_PORTD_DATA_R |= 0x08;		// Set E High
  GPIO_PORTD_DATA_R &= 0xF7;		// Set E Low
}

void out_LCD_high( INT8U Ch )
/*****************************************************************************
*   Input    : Mask
*   Output   : -
*   Function : Send high part of character to LCD.
*              This function works only in 4 bit data mode.
******************************************************************************/
{
  out_LCD_low((Ch & 0xF0) >> 4);
}



void clr_LCD()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Clear LCD.
******************************************************************************/
{


  wr_ctrl_LCD( 0x01 );
}


void home_LCD()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Return cursor to the home position.
******************************************************************************/
{
  wr_ctrl_LCD( 0x02 );
}

void Set_cursor( INT8U Ch )
/*****************************************************************************
*   Input    : New Cursor position
*   Output   : -
*   Function : Place cursor at given position.
******************************************************************************/
{
  wr_ctrl_LCD( Ch );
}


void out_LCD( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write control data to LCD.
******************************************************************************/
{
  INT16U i;

  out_LCD_high( Ch );
  for(i=0; i<1000; i++);
  out_LCD_low( Ch );
}

//void set_position_of_cursor(INT8U position)
//{
//    home_LCD();
//    int i;
//    for(i = 0; i < position; i++)
//    {
//        Set_cursor(0x14);
//        vTaskDelay(2);
//    }
//}

void lcd_task(void* pvParameters)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
    INT8S ch = 0;
    INT8U state = 0;
    lcd_init();
    Q_LCD = xQueueCreate(QUEUE_LEN, sizeof(INT8S));

    while(1){
        switch( state )
        {
            case 0:
              if( xQueueReceive( Q_LCD, &ch, portTICK_RATE_MS) == pdTRUE )
              {
                switch( ch )
                {
                  case 0xFF:
                    clr_LCD();
                    break;
                  case ESC:
                    state = 1;
                    break;
                  default:
                    out_LCD( ch );
                }
              }
              break;

            case 1:
              if( xQueueReceive( Q_LCD, &ch, portTICK_RATE_MS) == pdTRUE )
              {
                if( ch & 0x80 )
                {
                    Set_cursor( ch );
                }
                else
                {
                  switch( ch )
                  {
                    case '@':
                        home_LCD();
                      break;
                  }
                }
                state = 0;
              }
              break;
          }
        vTaskDelay(1);

    }
}


/****************************** End Of Module *******************************/




