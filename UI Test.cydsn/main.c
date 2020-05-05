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
#include "stdbool.h"

#include <ssd1306.h>
#include <ds3231.h>

#define DISPLAY_ADDRESS 0x3C // 011110+SA0+RW - 0x3C or 0x3D


// flag to tell whether a switch press was caught
bool enc_sw_flag = false;

// dislay states
#define STARTUP 0
#define LOGIN 1
#define RECORD_SELECT 2
#define RECORD_DETAIL 3
#define TYPE_CREDENTIAL 7
#define VIEW_CREDENTIALS 4
#define RECORD_TOTP 5
#define MANAGE_RECORDS 6 // over serial
uint8 display_state = STARTUP;


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
int detail_menu_size = 5;
char detail_menu_items[][20] = {
    "type username",
    "type password",
    "view credentials",
    "view TOTP code",
    "<- go back"
};
// menu coordinates
int detail_menu_top = 20;
int detail_menu_left = 20;


// struct to hold each item, when decrypted they should fit this format
// 64 bytes total (for now)
typedef struct {
    char title[9];
    char username[35];
    char password[19];
} Record;

Record null_record = {"","",""};

/********************************************************
*     KEYBOARD EMULATION STUFF                          *
********************************************************/

bool KB_Initialized = false;
int8 KB_Data[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // [0]: Modifiers [1]: reserved [2:7]: Key codes
typedef struct {
    int uid;
    bool uppercase;
} hid_usage;

// this will start at ascii 32 to get to keyboard values
hid_usage ascii_to_hid[] = {
    {0x2c, 0}, {0x1e, 1}, {0x34, 1}, {0x20, 1}, {0x21, 1}, {0x22, 1}, {0x24, 1}, {0x34, 0}, {0x26, 1}, {0x27, 1}, {0x25, 1}, {0x2e, 1}, {0x36, 0}, {0x2d, 0}, {0x37, 0}, {0x38, 0}, 
    {0x27, 0}, {0x1e, 0}, {0x1f, 0}, {0x20, 0}, {0x21, 0}, {0x22, 0}, {0x23, 0}, {0x24, 0}, {0x25, 0}, {0x26, 0}, {0x33, 1}, {0x33, 0}, {0x36, 1}, {0x2e, 0}, {0x37, 1}, {0x38, 1}, 
    {0x1f, 1}, {0x04, 1}, {0x05, 1}, {0x06, 1}, {0x07, 1}, {0x08, 1}, {0x09, 1}, {0x0a, 1}, {0x0b, 1}, {0x0c, 1}, {0x0d, 1}, {0x0e, 1}, {0x0f, 1}, {0x10, 1}, {0x11, 1}, {0x12, 1}, 
    {0x13, 1}, {0x14, 1}, {0x15, 1}, {0x16, 1}, {0x17, 1}, {0x18, 1}, {0x19, 1}, {0x1a, 1}, {0x1b, 1}, {0x1c, 1}, {0x1d, 1}, {0x2f, 0}, {0x31, 0}, {0x30, 0}, {0x23, 1}, {0x2d, 1}, 
    {0x35, 0}, {0x04, 0}, {0x05, 0}, {0x06, 0}, {0x07, 0}, {0x08, 0}, {0x09, 0}, {0x0a, 0}, {0x0b, 0}, {0x0c, 0}, {0x0d, 0}, {0x0e, 0}, {0x0f, 0}, {0x10, 0}, {0x11, 0}, {0x12, 0}, 
    {0x13, 0}, {0x14, 0}, {0x15, 0}, {0x16, 0}, {0x17, 0}, {0x18, 0}, {0x19, 0}, {0x1a, 0}, {0x1b, 0}, {0x1c, 0}, {0x1d, 0}, {0x2f, 1}, {0x31, 1}, {0x30, 1}, {0x35, 1}, {0x2a, 0}, 
};

void KB_TypeChar(hid_usage hid_id) {
    // send the current char
    while(!USBFS_1_bGetEPAckState(1));  			/* Wait for ACK before loading data */
    KB_Data[2] = hid_id.uid;
    KB_Data[0] = 2*(int)hid_id.uppercase;
    USBFS_1_LoadInEP(1, (uint8 *)KB_Data, 8); 	/* Load latest mouse data into EP1 and send to PC */
    
    // send a blank keystroke to clear the state of shift
    while(!USBFS_1_bGetEPAckState(1));  			/* Wait for ACK before loading data */
    KB_Data[2] = 0;
    KB_Data[0] = 0;
    USBFS_1_LoadInEP(1, (uint8 *)KB_Data, 8); 	/* Load latest mouse data into EP1 and send to PC */
}

void ascii_test() {
    for (unsigned int i=0;i<95;i++){
        LED_Write(0);
        KB_TypeChar(ascii_to_hid[i]);
        LED_Write(1);
    }
}

void KB_Initialize() {
    // usb keyboard setup
    // exit if we already intialized the keyboard
    if (KB_Initialized) return;
    display_clear();
    gfx_setCursor(20,20);
    gfx_println("initializing keyboard...");
    gfx_println("must be connected to micro-usb port for kb");
    gfx_println("push to go back");
    display_update();
    CyGlobalIntEnable; /* Enable global interrupts. */
    USBFS_1_Start(0, USBFS_1_DWR_VDDD_OPERATION);   /* Start USBFS Operation/device 0 and with 5V operation */ 	
	while(!USBFS_1_bGetConfiguration()) {      		/* Wait for Device to enumerate */
        if (enc_sw_flag) {
            enc_sw_flag = false;
            return;
        }
    }
    USBFS_1_LoadInEP(1, (uint8 *)KB_Data, 8); 	/* Loads an inital value into EP1 and sends it out to the PC */
    KB_Initialized = true;
}

void KB_TypeString(char* str_to_type, bool draw) {
    // enters the given string through keyboard emulation
    // takes a li bit of time
    KB_Initialize();
    if (KB_Initialized) {
        if (draw) {
            display_clear();
            gfx_setCursor(20,40);
            gfx_println("typing...");
            display_update();
        }
        for (unsigned int i = 0; i<strlen(str_to_type); i++) {
            // the ascii to hid table starts at ascii 32
            int shifted_ascii = (int) str_to_type[i] - 32;
            // type the current character
            KB_TypeChar(ascii_to_hid[shifted_ascii]);
        }
        if (draw) {
            gfx_setCursor(20,48);
            gfx_println("done!");
            display_update();
            CyDelay(400);
        }
    }
    else {
        display_clear();
        gfx_setCursor(20,40);
        gfx_println("failed, not connected");
        display_update();
        CyDelay(600);
    }
}

// UI Methods
void draw_record_list(Record * buff, int curr, int offset, int buff_size) {
    /* uses the record buff, the current record number,
        and the current buffer offset to print the current record menu */
    int buff_index = curr - offset;
    // handle overflow beyond bounds
    if (buff_index < 0) {
        Dial_SetCounter(0);
        buff_index = 0;
    } else if (buff_index > buff_size -1) {
        Dial_SetCounter(buff_size - 1);
        buff_index = buff_size - 1;
    }
    display_clear();
    gfx_fillRect(0, 22, 100, 20, WHITE);
    gfx_setTextSize(1);
    gfx_setCursor(0,6);
    gfx_setTextColor(WHITE);
    gfx_setTextSize(1);
    // only print these if we're not near the bottom end, otherwise "list end"
    gfx_println(buff_index >= 2 ? buff[buff_index-2].title : "list end");
    gfx_println(buff_index >= 1 ? buff[buff_index-1].title : "list end");
    gfx_setTextSize(2);
    gfx_setTextColor(BLACK);
    gfx_setCursor(5,24);
    gfx_println(buff[buff_index].title);
    gfx_setTextSize(1);
    gfx_setTextColor(WHITE);
    gfx_setCursor(0,42);
    // only print these if we're not near the top end, otherwise "list end"
    // TODO: Replace this with something that references the actual buffer size
    gfx_println(buff_index <= buff_size-2 ? buff[buff_index+1].title : "list end");
    gfx_println(buff_index <= buff_size-3 ? buff[buff_index+2].title : "list end");
    display_update();
}

void record_detail_update(int cursor) {
    if (cursor < 0) {
        Dial_SetCounter(0);
        cursor = 0;
    } else if (cursor > detail_menu_size -1) {
        Dial_SetCounter(detail_menu_size - 1);
        cursor = detail_menu_size - 1;
    }
    // this uses the coordinates of the menu from draw_record_detail
    gfx_fillRect(0,detail_menu_top,detail_menu_left,44,BLACK);
    gfx_drawCircle(10,24+9*cursor,2,WHITE);
    display_update();
}

void draw_record_detail(Record record) {
    display_clear();
    gfx_setCursor(0,6);
    gfx_setTextSize(1);
    gfx_setTextColor(WHITE);
    gfx_println(record.title);
    for (int i = 0; i<detail_menu_size; i++) {
        gfx_setCursor(detail_menu_left, detail_menu_top + 9*i);
        gfx_println(detail_menu_items[i]);
    }
    //gfx_setCursor(20,20);
    //gfx_println("type username");
    //gfx_setCursor(20,28);
    //gfx_println("type password");
    //gfx_setCursor(20,36);
    //gfx_println("view credentials");
    //gfx_setCursor(20,44);
    //gfx_println("get TOTP code");
    //gfx_setCursor(20,52);
    //gfx_println("<- go back");
    display_update();
    Dial_SetCounter(0);
    record_detail_update(Dial_GetCounter());
}

/*****************************
*       INTERRUPTS           *
*****************************/

CY_ISR(RX_INT) {
//    char rec_char = UART_1_ReadRxData();
//    UART_1_PutChar(rec_char);    // echo recieved character
//    display_clear();  
//    gfx_setTextSize(2);
//    gfx_setCursor(0,0);
//    gfx_setTextSize(8);
//    gfx_setTextColor(WHITE);
//    gfx_write(rec_char);
//    display_update();
}

CY_ISR(ENC_SW_INT) {
    //char send_buffer[10];
    //sprintf(send_buffer, "%d\n\r", Dial_GetCounter());
    //UART_1_PutString(send_buffer);
    //display_clear();  
    //display_update();
    //gfx_setTextSize(2);
    //gfx_setCursor(0,0);
    //gfx_setTextSize(8);
    //gfx_setTextColor(WHITE);
    //gfx_println(send_buffer);
    //display_update(); 
    enc_sw_flag = true;
}

CY_ISR(SW1_INT) {
    if (!KB_Initialized)
        KB_Initialize();
}

int main(void)
{
    LED_Write(1);
    CyDelay(100);
    LED_Write(0);
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    rx_int_StartEx(RX_INT);  // start RX interrupt (look for CY_ISR with RX_INT address)
    enc_sw_int_StartEx(ENC_SW_INT); // start encoder switch interrupt
    sw1_int_StartEx(SW1_INT); // start board switch interrupt

    UART_1_Start();
    UART_1_PutString("COM Port Open");
    
    // start quadrature encoder for rot. enc
    Dial_Start();
    
    // start i2c connection w oled
    I2COLED_Start();
    display_init(DISPLAY_ADDRESS);
    
    // load first items from memory
    int buff_offset = 0; // sd card location of first record
    int buff_size = 12;
    Record record_buffer[12] = {
        {"rec1", "username1", "password1"},
        {"rec2", "username2", "password2"},
        {"rec3", "username3", "password3"},
        {"rec4", "username4", "password4"},
        {"rec5", "username5", "password5"},
        {"rec6", "username6", "password6"},
        {"rec7", "username7", "password7"},
        {"rec8", "username8", "password8"},
        {"rec9", "username9", "password9"},
        {"rec10", "username10", "password10"},
        {"rec11", "username11", "password11"},
        {"rec12", "username12", "password12"},
    };
    
    // to store the currently selected record
    Record curr_record;
    
    // hold
    
    //draw_record_list(curr_record, prev_records, next_records);
    enc_sw_flag = false; // i guess the interrupt got run at first or somthing?
    

    for(;;)
    {
        
        // handle display updates
        switch (display_state) {
            case STARTUP:
                display_clear();
                gfx_setCursor(10,10);
                gfx_setTextSize(2);
                gfx_setTextColor(WHITE);
                gfx_println("LOGIN");
                display_update();
                display_state = LOGIN;
                CyDelay(1000);
            break;
            case LOGIN:
                if (true) {
                    char to_print[10];
                    sprintf(to_print, "%d", Dial_GetCounter());
                    display_clear();
                    gfx_setCursor(10,40);
                    gfx_println(to_print);
                    display_update();
                }
                if (enc_sw_flag == true) {
                    enc_sw_flag = false; // reset flag
                    display_state = RECORD_SELECT;
                    Dial_SetCounter(0);
                    draw_record_list(record_buffer, Dial_GetCounter(), buff_offset, buff_size);
                }
            break;
            case RECORD_SELECT:
                // handle any screen updates due to scrolling
                // this check is being glitchy so skipping it
                //if (Dial_GetCounter() != prev_dial) {
                if (true) {
                    // could load new values from SD card if nearing buffer ends
                    // if close to end of sd card, don't update--last item in buff
                    // should be last item on SD card.
                    draw_record_list(record_buffer, Dial_GetCounter(), buff_offset, buff_size);
                    curr_record = record_buffer[Dial_GetCounter() - buff_offset];
                };
                if (enc_sw_flag) {
                    enc_sw_flag = false; // reset flag
                    display_state = RECORD_DETAIL;
                    draw_record_detail(curr_record);
                }
            break;
            case RECORD_DETAIL:
                record_detail_update(Dial_GetCounter());
                if (enc_sw_flag) {
                    enc_sw_flag = false;
                    switch (Dial_GetCounter()) {
                        case RECORD_TYPE_USER: 
                        {
                            KB_TypeString(curr_record.username, true);
                            // typestring writes to screen, so redraw.
                            draw_record_detail(curr_record);
                        }
                        break;
                        case RECORD_TYPE_PW:
                        {
                            KB_TypeString(curr_record.password, true);
                            // typestring writes to screen, so redraw.
                            draw_record_detail(curr_record);
                        }
                        break;
                        case RECORD_VIEW_CREDENTIALS:
                        break;
                        case RECORD_VIEW_TOTP:
                        break;
                        case RECORD_BACK:
                        break;
                    }
                    
                    
                }
            break;
            case VIEW_CREDENTIALS:
            break;
            case RECORD_TOTP:
            break;
            case MANAGE_RECORDS:
            break;
        }
    }
}

/* [] END OF FILE */
