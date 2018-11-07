/*********************************************************************************/
/*                                                                               */
/*                                         designed by hamayan since 2009/05/21  */
/*********************************************************************************/
#ifndef  __DERIVERTIVE_H__
#define  __DERIVERTIVE_H__

/*************************************************************************/
/*                                                                       */
/*************************************************************************/
/* RED LED */
#define  LED_RED   PC6
/* BLUE LED */
#define  LED_BLUE  PC7

#define  GD1       PE12

/* CHARACTOR LCD */
#define  LCD_COLS  16
#define  LCD_RAWS  2

#define  LCD_RS  PB0
#define  LCD_RW  PB1
#define  LCD_E   PB5
#define  LCD_D4  PB12
#define  LCD_D5  PB13
#define  LCD_D6  PB14
#define  LCD_D7  PB15

enum WEEK_DAY { WEEK_Mon, WEEK_Tue, WEEK_Wed, WEEK_Thu, WEEK_Fri, WEEK_Sat, WEEK_Sun };
enum AM_PM { AM, PM };


/*************************************************************************/
/* task defines                                                          */
/*************************************************************************/
#define  MAX_TASK_NUMBER    3  //
enum GR_SAKURA_TSK_ID
{
  ID_stackMonitor,
  ID_redLedTask,
  ID_buleLedTask,
};

#define  MAX_MBX_NUMBER    1  //
enum GR_SAKURA_MBX_ID
{
  MBXID_dummy,         //
};

#define  MAX_SEM_NUMBER    2
enum GR_SAKURA_SEM_ID
{
  SEMID_I2C = 1,
  SEMID_SPI1,
};


#endif  /*__DERIVERTIVE_H__*/

/*********************************************************************************/
/* end of file                                                                   */
/*                                         designed by hamayan since 2009/05/21  */
/*********************************************************************************/
