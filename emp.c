/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: emp.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "emp_type.h"
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT8U    dummy1;
INT16S   dummy2;

/*****************************   Functions   *******************************/

void test1(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  dummy1++;
}

extern void test2(void)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  dummy2++;
}

/****************************** End Of Module *******************************/












