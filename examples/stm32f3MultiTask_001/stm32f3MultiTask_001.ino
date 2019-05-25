#include  <LiquidCrystal.h>
#include  <RTClock.h>
#include  "derivertive.h"

extern "C"
{
  #include  "mul_tsk.h"
}

/**********************************************************/
/* prototypes                                             */
/**********************************************************/
void tsk_ini( void );
void stackMonitor( void );
void redLedTask( void );
void buleLedTask( void );
void updateSystemTimer();

/**********************************************************/
/* valiables                                              */
/**********************************************************/
HardwareTimer systemTimer( 5 );
RTClock rtc( RTCSEL_LSE );  //RTClock rtc( RTCSEL_LSE, RTCSEL_LSE );
LiquidCrystal lcd( LCD_RS, LCD_RW, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7 );

SYSTIM  systim;
time_t unixTime;
time_t startUpTime;

uint8_t tsk0_stk[ 128 * 4 ];
uint8_t tsk1_stk[ 128 * 2 ];
uint8_t tsk2_stk[ 128 * 2 ];

/**********************************************************/
/* setup                                                  */
/**********************************************************/
void setup()
{
  pinMode( LED_BLUE, OUTPUT );
  lcd.begin( LCD_COLS, LCD_RAWS );
  lcd.clear();
  lcd.print( "STM32 Arduino." );
  lcd.setCursor( 1, 1 );
  lcd.print( "by hamayan." );

  delay( 1 * 1000UL );  /* 少し時間を入れないとシリアルから上手く出力しない！ */
  Serial1.begin( 115200UL );
  Serial1.println( "STM32 Arduino Multi Task Demo." );

  Serial.begin( 115200UL );
  while( !Serial.isConnected() )
  {
    digitalWrite( LED_BLUE, HIGH );
    delay( 500UL );
    digitalWrite( LED_BLUE, LOW );
    delay( 500UL );
  }
  Serial.println( "STM32 Arduino Multi Task Demo." );
   
  bkp_init();  /* initialized backup registers. */
  if( bkp_read(1) != 'h' || bkp_read(2) != 'a' || bkp_read(3) != 'm' || bkp_read(4) != 'a' )
  {
    struct tm_t localTime =
      {2018 - 1970,11,7,WEEK_Wed,PM,16,0,0};
    rtc.setTime( localTime );
    bkp_enable_writes();  /**/
    bkp_write( 1, 'h' ); bkp_write( 2, 'a' ); bkp_write( 3, 'm' ); bkp_write( 4, 'a' );
    bkp_disable_writes();
  }
  unixTime = rtc.getTime();
//  rtc.attachSecondsInterrupt( updateSecondTimer );  /* 何故かタスクが暴走する */

  systemTimer.pause();
  systemTimer.setPrescaleFactor( 72 );  /* 1Mhz timer clock. */
  systemTimer.setOverflow( 1000 - 1 );  /* 1ms interrupt */
  systemTimer.attachInterrupt( TIMER_UPDATE_INTERRUPT, updateSystemTimer );
  systemTimer.refresh();
  systemTimer.resume();

  tsk_ini();  //タスクの初期化
  sta_rdq( ID_stackMonitor );  //ラウンドロビン開始。ここからタスクが開始される
}

/**********************************************************/
/* loop but not use.                                      */
/**********************************************************/
void loop()
{
}

/**********************************************************/
/* task initilize                                         */
/**********************************************************/
void tsk_ini( void )
{
  reg_tsk( ID_stackMonitor, (void *)stackMonitor, (void *)tsk0_stk, sizeof(tsk0_stk), 0,0,0,0 );
  reg_tsk( ID_redLedTask, (void *)redLedTask, (void *)tsk1_stk, sizeof(tsk1_stk), 0,0,0,0 );
  reg_tsk( ID_buleLedTask, (void *)buleLedTask, (void *)tsk2_stk, sizeof(tsk2_stk), 0,0,0,0 );

  sta_tsk( ID_stackMonitor );
  sta_tsk( ID_redLedTask );
  sta_tsk( ID_buleLedTask );
}

/**********************************************************/
/* stack monitor task                                     */
/**********************************************************/
static unsigned int RemainStack( void *stk, unsigned int sz );
static void stackPrint( const char *msg, void *stk, unsigned int sz );

void stackMonitor( void )
{
  time_t startUpTimeBase = startUpTime + 10;
  while( 1 )
  {
    if( startUpTime >= startUpTimeBase )
    {
      startUpTimeBase = startUpTime + 10;
      stackPrint( "task1 stack : ", tsk1_stk, sizeof(tsk1_stk) );
      stackPrint( "task2 stack : ", tsk2_stk, sizeof(tsk2_stk) );
      stackPrint( "monitor stack : ", tsk0_stk, sizeof(tsk0_stk) );

      Serial1.println( dateTime() );
      Serial.println( dateTime() );
    }
    rot_rdq();
  }
}

static void stackPrint( const char *msg, void *stk, unsigned int sz )
{
  Serial1.print( msg );
  Serial1.print( RemainStack( stk, sz ), DEC );
  Serial1.print( "/" );
  Serial1.print( sz, DEC );
  Serial1.println( " used." );

  Serial.print( msg );
  Serial.print( RemainStack( stk, sz ), DEC );
  Serial.print( "/" );
  Serial.print( sz, DEC );
  Serial.println( " used." );
}

static unsigned int RemainStack( void *stk, unsigned int sz )
{
  unsigned int i;
  char *ptr = (char *)stk;

  for( i = 0; i < sz; i++ )
  {
    if( *ptr++ != 0 ) break;
  }

  return sz - i;
//  return i;
}

/**********************************************************/
/* Red Led Blink Task.                                    */
/**********************************************************/
void redLedTask()
{
  pinMode( LED_RED, OUTPUT );
  pinMode( GD1, OUTPUT );

  while( 1 )
  {
    if( digitalRead( LED_RED ) == LOW ) digitalWrite( LED_RED, HIGH );
    else digitalWrite( LED_RED, LOW );

    dly_tsk( 50UL );
  }
}

/**********************************************************/
/* Blue Led Blink Task.                                   */
/**********************************************************/
void buleLedTask( void )
{
  pinMode( LED_BLUE, OUTPUT );

  while( 1 )
  {
    if( digitalRead( LED_BLUE ) == LOW ) digitalWrite( LED_BLUE, HIGH );
    else digitalWrite( LED_BLUE, LOW );

    dly_tsk( 234UL );
  }
}

/**********************************************************/
/* return date and time stringd from RTC.                 */
/**********************************************************/
String dateTime()
{
  tm_t tm;
  rtc.breakTime( rtc.getTime(), tm );
  String str = "";
  str += tm.year + 1970;
  str += "/";
  str += tm.month;
  str += "/";
  str += tm.day;
  str += " ";
  str += tm.hour;
  str += ":";
  str += tm.minute;
  str += ":";
  str += tm.second;
//  str += " ";
//  str += unixTime;

  return str;
}

/**********************************************************/
/* update system timer handler                            */
/**********************************************************/
void updateSystemTimer()
{
  static int count = 0;
  systim++;
  if( ++count >= 1000 )
  {
    count = 0;
    updateSecondTimer();
  }
}

void updateSecondTimer()
{
  unixTime++;
  startUpTime++;
}
