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
#include <stdlib.h>

#include <ds3231.h>

// record stuff

// struct to hold each item, when decrypted they should fit this format
// 64 bytes total (for now)
// ALREADY IN OTHER FILE
typedef struct {
    char title[9];
    char username[35];
    char password[19];
} Record;
Record null_record = {"","",""};

/*******************************
*       UART STUFF             *
*******************************/

char rx_buffer[30];
int rx_buff_length;
char last_serial_cmd[30];
void clr_rx_buffer(){ 
    memset(rx_buffer, 0, 30); 
    rx_buff_length = 0;
};
void clr_last_serial_cmd(){ memset(last_serial_cmd, 0, 30); };
void update_rx_buffer(char to_add) {
    char str_to_add[2] = {to_add, 0};
    strcat(rx_buffer, str_to_add);
}

typedef struct {
    int size;
    char title[20];
    char items[10][20];
} SerialMenu;

void serial_print_menu(SerialMenu menu) {
    UART_1_PutString("\n\r");
    UART_1_PutString(menu.title);
    UART_1_PutString(":\n\r");
    for (int i = 0; i<menu.size; i++) {
        // print item number (max of 9 items)
        // just does it by ascii conversion
        UART_1_PutChar(i + 0x31);
        UART_1_PutString(". ");
        UART_1_PutString(menu.items[i]);
        UART_1_PutString("\n\r");
    }
    UART_1_PutString("\n\rEnter the choice you want: ");
}

void serial_print_record(Record record) {
    // prints a record's info to the serial monitor in a nice way
    UART_1_PutString("\n\r\n\r   ");
    for (int i=0; i<30; i++) {
        UART_1_PutChar('*');
    }
    UART_1_PutString("\n\r   * Title: ");
    UART_1_PutString(record.title);
    UART_1_PutString("\n\r   * Username: ");
    UART_1_PutString(record.username);
    UART_1_PutString("\n\r   * Password: ");
    UART_1_PutString(record.password);
    UART_1_PutString("\n\r   ");
    for (int i=0; i<30; i++) {
        UART_1_PutChar('*');
    }
}

void serial_inv_cmd() {
    UART_1_PutString("\n\rInvalid Command!");
}

// MENU/Serial State DEFINITIONS
#define S_WAIT -1
#define S_STARTUP 0
#define S_MAIN_MENU 1
#define S_ADD_RECORD 2

// Serial state variables
int s_main_state = S_MAIN_MENU;
int s_sec_state = 0; // secondary state

// Serial Main Menu
SerialMenu main_menu = {
    3,
    "Main Menu",
    {
        "Add new record",
        "View all records",
        "Generate Password"
    }
};

// definitions for commands for serial main menu
#define INVALID_CMD 0
#define S_MAIN_ADD 1
#define S_MAIN_VIEW 2
#define S_MAIN_GEN_PW 3

// states for adding a record
#define ADD_REC_START 0
#define ADD_REC_TITLE 1
#define ADD_REC_UNAME 2
#define ADD_REC_PASS_MENU 3
#define ADD_REC_PASS_ENTRY 4
#define ADD_REC_CONFIRM 5

SerialMenu pass_menu = {
    2,
    "Password Menu",
    {
        "Enter Password",
        "Generate Password"
    }
};

// command definitions for password menu
#define PASS_MENU_ENTER 1
#define PASS_MENU_GEN 2

// generate a password
void generate_password(char* out) {
    /* generate a 16-character ascii
        password using the PRS component
    */
    char current_char = 0;
    for (int i = 0; i<16; i++) {
        current_char = (char) PRS_Read();
        while ((int) current_char < 33 || (int) current_char > 126) {
            // if outside of valid range of characters, generate new character
            // spaces are not allowed, to include them change 33 to 32
            PRS_Step();
            current_char = (char) PRS_Read();
        }
        out[i] = current_char;
        // move to next PRS value
        PRS_Step();
    }
    out[16] = 0; // null terminate password
}

// for the rtc, just debugging stuff
void serial_print_dt(DateTime dt) {
    UART_1_PutString("\n\rdatetime: ");
    char to_send[40];
    sprintf(to_send, "%d-%d-%d %d:%d:%2.0d unix: %d", dt_getYear(dt), \
        dt_getMonth(dt), dt_getDay(dt), dt_getHour(dt), \
        dt_getMinute(dt), dt_getSecond(dt), dt_getUnixtime(dt));
    UART_1_PutString(to_send);
}

CY_ISR(RX_INT) {
    char rec_char = UART_1_ReadRxData();
    if (rec_char == 0xD) {
        // move buffer into last_serial_cmd on enter
        strcpy(last_serial_cmd, rx_buffer);
        clr_rx_buffer();
    }       
    else {
        UART_1_PutChar(rec_char);    // echo recieved character
        // add non-enter characters to rx buffer
        rx_buffer[rx_buff_length] = rec_char;
        rx_buffer[rx_buff_length + 1] = 0;
        rx_buff_length += 1;
    }
}

CY_ISR(ENC_SW_INT) {
    char send_buffer[10];
    sprintf(send_buffer, "%d\n\r", Dial_GetCounter());
    UART_1_PutString(send_buffer);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    rx_int_StartEx(RX_INT);  // start RX interrupt (look for CY_ISR with RX_INT address)
    enc_sw_int_StartEx(ENC_SW_INT); // start encoder switch interrupt

    UART_1_Start();
    UART_1_PutString("COM Port Open");
    rx_buff_length = 0;
    
    Dial_Start();
    s_main_state = S_STARTUP;
    
    // to store the currently selected record
    // for serial applications, its the record in progress
    Record curr_record;
    
    // start the rtc communication over i2c
    I2CRTC_Start();
    I2CRTC_EnableInt();
    
    // get the current time
    DateTime now = RTC_now();
    
    // start generating random numbers
    PRS_Start();
    PRS_WriteSeed(dt_getUnixtime(now));

    for(;;)
    {
        // step to the next random value
        PRS_Step();
        
        LED_Write(last_serial_cmd[0] != 0);
        switch (s_main_state) {
            case S_WAIT:
            break;
            case S_STARTUP:
                serial_print_menu(main_menu);
                s_main_state = S_MAIN_MENU;
            break;
            case S_MAIN_MENU:
                if (last_serial_cmd[0] != 0) {
                    int cmd = atoi(last_serial_cmd);
                    switch(cmd) {
                        // first case is if atoi returned 0
                        // ie letters, etc
                        case INVALID_CMD:
                            serial_inv_cmd();
                            serial_print_menu(main_menu);
                        break;
                        case S_MAIN_ADD:
                            s_main_state = S_ADD_RECORD;
                            s_sec_state = ADD_REC_START;
                            UART_1_PutString("\n\rAdd record...");
                        break;
                        default:
                            {
                            UART_1_PutString("\n\rGot Command: ");
                            char to_send[5];
                            sprintf(to_send, "%d %s", cmd, last_serial_cmd);
                            UART_1_PutString(to_send);
                            serial_print_menu(main_menu);
                            }
                        break;
                    }
                    // always reset last_serial_cmd
                    // after handling an inputted command
                    //last_serial_cmd[0] = 0;
                    clr_last_serial_cmd();
                };
            break;
            case S_ADD_RECORD:
                switch(s_sec_state) {
                    case ADD_REC_START:
                        curr_record = null_record;
                        UART_1_PutString("\n\rEnter Record Title: ");
                        s_sec_state = ADD_REC_TITLE;
                    break;
                    case ADD_REC_TITLE:
                        // once something has been entered
                        if (last_serial_cmd[0] != 0) {
                            // update curr record with title
                            strcpy(curr_record.title, last_serial_cmd);
                            clr_last_serial_cmd();
                            // set up for username entry
                            UART_1_PutString("\n\rEnter username for ");
                            UART_1_PutString(curr_record.title);
                            UART_1_PutString(": ");
                            s_sec_state = ADD_REC_UNAME;
                        }
                    break;
                    case ADD_REC_UNAME:
                        // once a title has been entered
                        if (last_serial_cmd[0] != 0) {
                            // update curr record with username
                            strcpy(curr_record.username, last_serial_cmd);
                            clr_last_serial_cmd();
                            // set up for pw entry
                            UART_1_PutString("\n\rPassword for user ");
                            UART_1_PutString(curr_record.username);
                            UART_1_PutString(": ");
                            serial_print_menu(pass_menu);
                            s_sec_state = ADD_REC_PASS_MENU;
                        }
                    break;
                    case ADD_REC_PASS_MENU:
                        if (last_serial_cmd[0] != 0) {
                            // listen for menu entry
                            int cmd = atoi(last_serial_cmd);
                            switch(cmd) {
                                // first case is if atoi returned 0
                                // ie letters, etc
                                case INVALID_CMD:
                                    serial_inv_cmd();
                                    serial_print_menu(pass_menu);
                                break;
                                case PASS_MENU_ENTER:
                                    UART_1_PutString("\n\rEnter Password for user ");
                                    UART_1_PutString(curr_record.username);
                                    UART_1_PutString(": ");
                                    s_sec_state = ADD_REC_PASS_ENTRY;
                                break;
                                case PASS_MENU_GEN:
                                    {
                                    UART_1_PutString("\n\rGenerating password! ");
                                    
                                    // generate password
                                    char pass[17];
                                    generate_password(pass);
                                    strcpy(curr_record.password, pass);
                                    UART_1_PutString("\n\rGenerated Password for user ");
                                    UART_1_PutString(curr_record.username);
                                    UART_1_PutString(": ");
                                    UART_1_PutString(curr_record.password);
                                    s_sec_state = ADD_REC_CONFIRM;
                                    }
                                break;
                            }
                            clr_last_serial_cmd();
                        }
                    break;
                    case ADD_REC_PASS_ENTRY:
                        // password entry option, password entered
                        if (last_serial_cmd[0] != 0) {
                            // update curr record with username
                            strcpy(curr_record.password, last_serial_cmd);
                            clr_last_serial_cmd();
                            // echo full record and prompt user to confirm
                            serial_print_record(curr_record);
                            UART_1_PutString("\n\r\n\rEnter 'y' to confirm, anything else to cancel.");
                            s_sec_state = ADD_REC_CONFIRM;
                        }
                    break;
                    case ADD_REC_CONFIRM:
                        // once a password is entered, confirm
                        if (last_serial_cmd[0] != 0) {
                            // update curr record with username
                            strcpy(curr_record.password, last_serial_cmd);
                            clr_last_serial_cmd();
                            UART_1_PutString("\n\rCancelling!");
                            s_main_state = S_MAIN_MENU;
                            serial_print_menu(main_menu);
                        }
                    break;
                }
            break;
        }
        // get latest data recieved
        //Ch = UART_1_GetChar();
    }
}

/* [] END OF FILE */
