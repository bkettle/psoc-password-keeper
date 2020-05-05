/* 
    holds functinos 
    for the OLED UI
*/

#ifndef PK_UI
#define PK_UI
    
#include <project.h>
#include <ssd1306.h>
#include <pk_record.h>
#include <pk_usb.h>
#include <pk_sdrecorddb.h>
#include <pk_serialfuncs.h>
#include <stdbool.h>
#include <stdio.h>
    
// address for the oled
#define DISPLAY_ADDRESS 0x3C // 011110+SA0+RW - 0x3C or 0x3D
    
    
// dislay states
#define STARTUP 0
#define LOGIN 1
#define RECORD_SELECT 2
#define RECORD_DETAIL 3
#define TYPE_CREDENTIAL 7
#define VIEW_CREDENTIALS 4
#define RECORD_TOTP 5
#define MANAGE_RECORDS 6 // over serial
uint8 display_state;
    
    
/***************************************************
*       VALUES FOR RECORD DETAIL MENU              *
***************************************************/
// cursor values for detail menu
// man I hate having these things defined
// in so many places, there's gotta be a better way
// a menu class or something would definitely be best
// that returns something when it's selected
// OH C doesn't have classes lol that's easier
#define RECORD_TYPE_USER 0
#define RECORD_TYPE_PW 1
#define RECORD_VIEW_CREDENTIALS 2
#define RECORD_VIEW_TOTP 3
#define RECORD_BACK 4

// menu items for record detail menu
int detail_menu_size;
char detail_menu_items[5][20];
// menu coordinates
int detail_menu_top;
int detail_menu_left;

// hold current records for menu
int buff_offset; // sd card location of first record
int buff_size; // length of the buffer
int num_loaded; // number of records currently loaded in buff
Record record_buffer[12];

// to store the currently selected record
Record curr_record;

// store the previous index to calculate if a change occurred
int prev_index;

// UI Methods
void ui_draw_recordList(Record * buff, int curr_index);

void ui_recordDetailUpdate(int cursor);

void ui_drawRecordDetail(Record record);

void ui_fsm(bool * enc_sw_flag);


    
#endif

/* [] END OF FILE */
