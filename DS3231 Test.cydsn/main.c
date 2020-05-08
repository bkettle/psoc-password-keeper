/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "stdio.h"
#include "ds3231.h"

void serial_print_dt(DateTime dt) {
    UART_1_PutString("\n\rdatetime: ");
    char to_send[40];
    sprintf(to_send, "%d-%d-%d %d:%d:%2.0d unix: %d", dt_getYear(dt), \
        dt_getMonth(dt), dt_getDay(dt), dt_getHour(dt), \
        dt_getMinute(dt), dt_getSecond(dt), dt_getUnixtime(dt));
    UART_1_PutString(to_send);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    UART_1_Start();
    UART_1_PutString("COM Port Open");
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    I2CRTC_Start();
    I2CRTC_EnableInt();
    
    
    // HAVE TO SET TO UTC TIME
    DateTime init = {
        20,     // year from 2000
        5,      // month
        8,      // day
        20,      // hour
        31,      // minute
        20      // second
    };
    RTC_setTime(init); //TIME SHOULD BE SET NOW

    for(;;)
    {
        DateTime now;
        now = RTC_now();
        serial_print_dt(now);
        CyDelay(1000);
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
