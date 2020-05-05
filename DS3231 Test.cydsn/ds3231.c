/* Very simple library for DS3231
    on PSoC, inspired by Adafruit's
    RTClib. Only crucial functionality
    is ported over.
*/

#include <project.h>
#include <stdlib.h>
#include <stdbool.h>

#include <ds3231.h>

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

// this stuff should go in the header but that's not working
uint8 RTC_read_buff[10];

// return the year from a DateTime struct
uint16_t dt_getYear(DateTime dt) { return 2000 + dt.yOff; }
// return the month from a DateTime struct
uint8_t dt_getMonth(DateTime dt) { return dt.m; }
// return the day from a DateTime struct
uint8_t dt_getDay(DateTime dt) { return dt.d; }
// return the hour from a DateTime struct
uint8_t dt_getHour(DateTime dt) { return dt.hh; }
// return the minute from a DateTime struct
uint8_t dt_getMinute(DateTime dt) { return dt.mm; }
// return the second from a DateTime struct
uint8_t dt_getSecond(DateTime dt) { return dt.ss; }
// return whether the time is PM from a DateTime struct
uint8_t dt_isPM(DateTime dt) { return dt.hh >= 12; }

/**************************************************************************/
/*!
    @brief  Read a byte from an I2C register
    @param addr I2C address
    @param reg Register address
    @return Register value
*/
/**************************************************************************/
uint8_t read_i2c_register(uint8_t addr, uint8_t reg) {
    I2CRTC_MasterSendStart(addr,0); // send write command to addr
    I2CRTC_MasterWriteByte(reg);
    I2CRTC_MasterSendStop();

    I2CRTC_MasterReadBuf(addr, RTC_read_buff, 1, I2CRTC_MODE_COMPLETE_XFER);
    return RTC_read_buff[0];
}

/**************************************************************************/
/*!
    @brief  Write a byte to an I2C register
    @param addr I2C address
    @param reg Register address
    @param val Value to write
*/
/**************************************************************************/
void write_i2c_register(uint8_t addr, uint8_t reg, uint8_t val) {
  I2CRTC_MasterSendStart(addr,0); // send write command to addr
  I2CRTC_MasterWriteByte(reg);
  I2CRTC_MasterWriteByte(val);
  I2CRTC_MasterSendStop();
}



/**************************************************************************/
// utility code, some of this could be exposed in the DateTime API if needed
/**************************************************************************/

/**
  Number of days in each month, from January to November. December is not
  needed. Omitting it avoids an incompatibility with Paul Stoffregen's Time
  library. C.f. https://github.com/adafruit/RTClib/issues/114
*/
const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};

/**************************************************************************/
/*!
    @brief  Given a date, return number of days since 2000/01/01,
            valid for 2000--2099
    @param y Year
    @param m Month
    @param d Day
    @return Number of days
*/
/**************************************************************************/
uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
  if (y >= 2000)
    y -= 2000;
  uint16_t days = d;
  for (uint8_t i = 1; i < m; ++i)
    days += pgm_read_byte(daysInMonth + i - 1);
  if (m > 2 && y % 4 == 0)
    ++days;
  return days + 365 * y + (y + 3) / 4 - 1;
}

/**************************************************************************/
/*!
    @brief  Given a number of days, hours, minutes, and seconds, return the
   total seconds
    @param days Days
    @param h Hours
    @param m Minutes
    @param s Seconds
    @return Number of seconds total
*/
/**************************************************************************/
uint32_t time2ulong(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
  return ((days * 24UL + h) * 60 + m) * 60 + s;
}

// these were adapted from DateTime classes to build DateTime structs
/**************************************************************************/
/*!
    @brief  Constructor from
        [Unix time](https://en.wikipedia.org/wiki/Unix_time).
    This builds a DateTime from an integer specifying the number of seconds
    elapsed since the epoch: 1970-01-01 00:00:00. This number is analogous
    to Unix time, with two small differences:
     - The Unix epoch is specified to be at 00:00:00
       [UTC](https://en.wikipedia.org/wiki/Coordinated_Universal_Time),
       whereas this class has no notion of time zones. The epoch used in
       this class is then at 00:00:00 on whatever time zone the user chooses
       to use, ignoring changes in DST.
     - Unix time is conventionally represented with signed numbers, whereas
       this constructor takes an unsigned argument. Because of this, it does
       _not_ suffer from the
       [year 2038 problem](https://en.wikipedia.org/wiki/Year_2038_problem).
    If called without argument, it returns the earliest time representable
    by this class: 2000-01-01 00:00:00.
    @see The `unixtime()` method is the converse of this constructor.
    @param t Time elapsed in seconds since 1970-01-01 00:00:00.
*/
/**************************************************************************/
DateTime dt_fromUnix(uint32_t t) {
  t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

  DateTime dt = {};
  dt.ss = t % 60;
  t /= 60;
  dt.mm = t % 60;
  t /= 60;
  dt.hh = t % 24;
  uint16_t days = t / 24;
  uint8_t leap;
  for (dt.yOff = 0;; ++dt.yOff) {
    leap = dt.yOff % 4 == 0;
    if (days < 365U + leap)
      break;
    days -= 365 + leap;
  }
  for (dt.m = 1; dt.m < 12; ++dt.m) {
    uint8_t daysPerMonth = pgm_read_byte(daysInMonth + dt.m - 1);
    if (leap && dt.m == 2)
      ++daysPerMonth;
    if (days < daysPerMonth)
      break;
    days -= daysPerMonth;
  }
  dt.d = days + 1;

  return dt;
}

/**************************************************************************/
/*!
    @brief  Constructor from (year, month, day, hour, minute, second).
    @warning If the provided parameters are not valid (e.g. 31 February),
           the constructed DateTime will be invalid.
    @see   The `isValid()` method can be used to test whether the
           constructed DateTime is valid.
    @param year Either the full year (range: 2000--2099) or the offset from
        year 2000 (range: 0--99).
    @param month Month number (1--12).
    @param day Day of the month (1--31).
    @param hour,min,sec Hour (0--23), minute (0--59) and second (0--59).
*/
/**************************************************************************/
DateTime dt_fromYMD(uint16_t year, uint8_t month, uint8_t day, uint8_t hour,
                   uint8_t min, uint8_t sec) {
  DateTime dt = {};
  if (year >= 2000)
    year -= 2000;
  dt.yOff = year;
  dt.m = month;
  dt.d = day;
  dt.hh = hour;
  dt.mm = min;
  dt.ss = sec;
  return dt;
}
               
/**************************************************************************/
/*!
    @brief  Return Unix time: seconds since 1 Jan 1970.
    @see The `DateTime::DateTime(uint32_t)` constructor is the converse of
        this method.
    @return Number of seconds since 1970-01-01 00:00:00.
*/
/**************************************************************************/
uint32_t dt_getUnixtime(DateTime dt) {
  uint32_t t;
  uint16_t days = date2days(dt.yOff, dt.m, dt.d);
  t = time2ulong(days, dt.hh, dt.mm, dt.ss);
  t += SECONDS_FROM_1970_TO_2000; // seconds from 1970 to 2000

  return t;
}

/**************************************************************************/
/*!
    @brief  Convert the DateTime to seconds since 1 Jan 2000
    The result can be converted back to a DateTime with:
    ```cpp
    DateTime(SECONDS_FROM_1970_TO_2000 + value)
    ```
    @return Number of seconds since 2000-01-01 00:00:00.
*/
/**************************************************************************/
uint32_t dt_getSecondstime(DateTime dt) {
  uint32_t t;
  uint16_t days = date2days(dt.yOff, dt.m, dt.d);
  t = time2ulong(days, dt.hh, dt.mm, dt.ss);
  return t;
}

/**************************************************************************/
/*!
    @brief  Convert a binary coded decimal value to binary. RTC stores time/date
   values as BCD.
    @param val BCD value
    @return Binary value
*/
/**************************************************************************/
uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }

/**************************************************************************/
/*!
    @brief  Convert a binary value to BCD format for the RTC registers
    @param val Binary value
    @return BCD value
*/
/**************************************************************************/
uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }

// DS3231 stuff!
/**************************************************************************/
/*!
    @brief  Set the date and flip the Oscillator Stop Flag
    @param dt DateTime object containing the date/time to set
*/
/**************************************************************************/
void RTC_setTime(DateTime dt) {
    //I2CRTC_MasterSendStart(DS3231_ADDRESS,0); // send write command to addr
    //I2CRTC_MasterWriteByte(DS3231_TIME);// start at location 0
    uint8 tx_buff[8] = {
        0,
        bin2bcd(dt_getSecond(dt)),
        bin2bcd(dt_getMinute(dt)),
        bin2bcd(dt_getHour(dt)),
        0,
        bin2bcd(dt_getDay(dt)),
        bin2bcd(dt_getMonth(dt)),
        bin2bcd(dt_getYear(dt)-2000),
    };
    I2CRTC_MasterWriteBuf(DS3231_ADDRESS,tx_buff,8,I2CRTC_MODE_COMPLETE_XFER );   // Send read start
    while(0u == (I2CRTC_MasterStatus() & I2CRTC_MSTAT_WR_CMPLT));      // Waiting for writing
    I2CRTC_MasterClearStatus(); 
}

/**************************************************************************/
/*!
    @brief  Get the current date/time
    @return DateTime object with the current date/time
*/
/**************************************************************************/
DateTime RTC_now() {
    uint8 tx_buff[1] = {0};
    I2CRTC_MasterWriteBuf(DS3231_ADDRESS,tx_buff,1,I2CRTC_MODE_COMPLETE_XFER );   // Send read start
    while(0u == (I2CRTC_MasterStatus() & I2CRTC_MSTAT_WR_CMPLT));      // Waiting for writing
    I2CRTC_MasterClearStatus();  
    
    uint8 resp = I2CRTC_MasterReadBuf(DS3231_ADDRESS, RTC_read_buff, 7, I2CRTC_MODE_COMPLETE_XFER);
    while(0u == (I2CRTC_MasterStatus() & I2CRTC_MSTAT_RD_CMPLT));      // Wait for read to finish
    I2CRTC_MasterClearStatus();
    
    uint8_t ss = bcd2bin(RTC_read_buff[0] & 0x7F);
    uint8_t mm = bcd2bin(RTC_read_buff[1]);
    uint8_t hh = bcd2bin(RTC_read_buff[2]);
    uint8_t d = bcd2bin(RTC_read_buff[4]);
    uint8_t m = bcd2bin(RTC_read_buff[5]);
    uint16_t yOff = bcd2bin(RTC_read_buff[6]);
    I2CRTC_MasterClearReadBuf();

    DateTime dt = {yOff, m, d, hh, mm, ss};
    return dt;
}

// sqw, alarm and temperature functionality not ported yet, see adafruit RTClib

/* [] END OF FILE */
