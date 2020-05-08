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
#include <FS.h>
#include <string.h>
#include <Global.h>
#include <stdbool.h>

#include <ds3231.h>
#include <ssd1306.h>

#include <pk_record.h>
#include <pk_sdrecorddb.h>
#include <pk_serialfuncs.h>
#include <pk_ui.h>

// flag for whether the dial's switch was pushed
bool enc_sw_flag;
bool sw1_flag; // flag for the onboard switch

CY_ISR(ENC_SW_INT) {
    enc_sw_flag = true;
    UART_1_PutString("enc pushed\n\r");
}

CY_ISR(SW1_INT) {
    // toggle flag
    // this switch just used as debug
    // toggle right now
    sw1_flag = !sw1_flag;
}

int main() {
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    // this causes issues when plugged into the microusb, it gets phantom interrupts
    // maybe we can enable it sometimes and disable other times
    //rx_int_StartEx(RX_INT);  // start RX interrupt (look for CY_ISR with RX_INT address)
    enc_sw_int_StartEx(ENC_SW_INT); // start encoder switch interrupt
    sw1_int_StartEx(SW1_INT); // start encoder switch interrupt
    
    /* Start LCD */
    //LCD_Start();
    UART_1_Start();
    
    /* Initialize file system */
    FS_Init();
    
    Dial_Start();
    s_main_state = S_STARTUP;
    
    // to store the currently selected record
    // for serial applications, its the record in progress
    Record curr_record;

    // start the rtc communication over i2c
    I2CRTC_Start();
    I2CRTC_EnableInt();
    
    // start i2c connection w oled
    I2COLED_Start();
    I2COLED_EnableInt();
    display_init(DISPLAY_ADDRESS);
    
    // get the current time
    DateTime now = RTC_now();
    
    // start generating random numbers
    PRS_Start();
    PRS_WriteSeed(dt_getUnixtime(now));
    
    // format the sd card and create the db
    //SD_createDB(false);
    //Record new_rec = {"bbb", "bbbb", "bbbbb"};
    //SD_addRecord(&new_rec);
    serial_menu_init();
    
    // show startup message on display
    gfx_setCursor(10,10);
    gfx_setTextSize(2);
    gfx_setTextColor(WHITE);
    gfx_println("STARTUP");
    display_update();
    USB_Initialize();
    
    USB_print("STARTUP\n\r");
    
    for(;;) {
        // update the UI record buffer if the serial thing
        // has just added a record to the db
        if (serial_status == S_STATUS_REQUIRE_RELOAD) {
            serial_status = S_STATUS_GOOD;
            ui_forceBufferUpdate = true;
        }
        
        // CALL FSMs
        // only do the serial things if serial is connected
        // ie the user has putty open
        if (USB_isReady()) serial_fsm(&curr_record);
        ui_fsm(&enc_sw_flag);
        enc_sw_flag = false; // ui fsm handled any encoder input        
    }
}

/* [] END OF FILE */

