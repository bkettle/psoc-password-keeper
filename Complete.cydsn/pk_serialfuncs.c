/* defines functions
    for interfacing with the user
    over serial.
*/
    
#include <pk_serialfuncs.h>
#include <pk_sdrecorddb.h>
#include <pk_usb.h>

#include "base32.h" // for decoding/encoding base32
/*******************************
*       UART STUFF             *
*******************************/


// last serial command holds the previous command
// while rx_buffer holds the one currently being
// typed. last serial command is updated on enter
// rx_buffer is also defined in pk_usb
char rx_buffer[40];
int rx_buff_length;
char last_serial_cmd[40];
int last_serial_cmd_length = 40;
//void clr_rx_buffer(){ 
//    memset(rx_buffer, 0, 30); 
//    rx_buff_length = 0;
//};
void clr_last_serial_cmd(){ 
    memset(last_serial_cmd, 0, last_serial_cmd_length); 
}

Record new_record = {"","","",false,""};

// Serial Main Menu
SerialMenu main_menu = {
    3,
    "Main Menu",
    {
        "Add new record",
        "View all records",
        "Clear Password DB"
    }
};

// password creation menu
SerialMenu pass_menu = {
    2,
    "Password Menu",
    {
        "Enter Password",
        "Generate Password"
    }
};

// STATES AND THINGS
// FOR SERIAL MENUS
// main fsm defs in header 
// definitions for commands for serial main menu
#define INVALID_CMD 0
#define S_MAIN_ADD 1
#define S_MAIN_VIEW 2
#define S_MAIN_FORMAT 3

// states for adding a record
#define ADD_REC_START 0
#define ADD_REC_TITLE 1
#define ADD_REC_UNAME 2
#define ADD_REC_PASS_MENU 3
#define ADD_REC_PASS_ENTRY 4
#define ADD_REC_PROMPT_TOTP 5
#define ADD_REC_TOTP_ENTRY 6
#define ADD_REC_CONF_PROMPT 7
#define ADD_REC_CONFIRM 8

// command definitions for password menu
#define PASS_MENU_ENTER 1
#define PASS_MENU_GEN 2

// HOLDS THE RECORD BEING CREATED
Record new_record;
// END STATES AND THINGS

void update_rx_buffer(char to_add) {
    char str_to_add[2] = {to_add, 0};
    strcat(rx_buffer, str_to_add);
}

void serial_print_menu(SerialMenu menu) {
    USB_print("\n\r");
    USB_print(menu.title);
    USB_print(":\n\r");
    for (int i = 0; i<menu.size; i++) {
        // print item number (max of 9 items)
        // just does it by ascii conversion
        char itemnum[2] = {(char) (i + 0x31), 0};
        USB_print(itemnum);
        USB_print(". ");
        USB_print(menu.items[i]);
        USB_print("\n\r");
    }
    USB_print("\n\rEnter the choice you want: ");
}

void serial_print_record(Record record) {
    // prints a record's info to the serial monitor in a nice way
    USB_print("\n\r\n\r   ");
    char star_line[30] = {0};
    for (int i=0; i<29; i++) {
        star_line[i] = '*';
    }
    star_line[29] = 0;
    USB_print(star_line);
    USB_print("\n\r   * Title: ");
    USB_print(record.title);
    USB_print("\n\r   * Username: ");
    USB_print(record.username);
    USB_print("\n\r   * Password: ");
    USB_print(record.password);
    USB_print("\n\r   * TOTP: ");
    USB_print(record.totp_enabled ? "Enabled" : "Disabled");
    USB_print("\n\r   ");
    USB_print(star_line);
}

void serial_printAllRecords() {
    // prints all records on the sd card
    int sd_size = SD_getNumRecords();
    Record curr; // holds the current record to be printed
    for (int i=0; i<sd_size; i++) {
        SD_readRecords(i, 1, &curr);
        serial_print_record(curr);
    }
}

void serial_inv_cmd() {
    USB_print("\n\rInvalid Command!");
}

// print hex of a char array
void serial_printHex(unsigned char * arr, size_t size) {
    char tp[3]; //holds things to be printed
    USB_print("0x");
    for (uint i=0; i<size; i++) {
        sprintf(tp, "%X", arr[i]);
        USB_print(tp);
    }
    USB_print(" ");
}

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
    USB_print("\n\rdatetime: ");
    char to_send[40];
    sprintf(to_send, "%d-%d-%d %d:%d:%2.0d unix: %d", dt_getYear(dt), \
        dt_getMonth(dt), dt_getDay(dt), dt_getHour(dt), \
        dt_getMinute(dt), dt_getSecond(dt), dt_getUnixtime(dt));
    USB_print(to_send);
}

// this ISR is unnecessary because we don't use the UART
// anymore. Need to handle USB Serial commands another way
// though.
// Input is now handled by pk_usb USB_handleInput,
// this is just here for fun
//CY_ISR(RX_INT) {
//    char rec_char = UART_1_ReadRxData();
//    if (rec_char == 0xD) {
//        // move buffer into last_serial_cmd on enter
//        strcpy(last_serial_cmd, rx_buffer);
//        clr_rx_buffer();
//    }       
//    else {
//        UART_1_PutChar(rec_char);    // echo recieved character
//        // add non-enter characters to rx buffer
//        rx_buffer[rx_buff_length] = rec_char;
//        rx_buffer[rx_buff_length + 1] = 0;
//        rx_buff_length += 1;
//    }
//}

/****************************************
*         SERIAL MENU FSM               *
****************************************/

// Serial state variables
int s_main_state = S_MAIN_MENU;
int s_sec_state = 0; // secondary state
int serial_status = S_STATUS_GOOD;

void serial_menu_init() {
    //rx_buff_length = 0;
    s_main_state = S_STARTUP;
}

void serial_fsm() {
    // step to the next random value
    PRS_Step();
    // update last_serial_cmd based on input
    USB_handleInput(last_serial_cmd);
    
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
                        USB_print("\n\rAdd record...");
                    break;
                    case S_MAIN_VIEW:
                        s_main_state = S_STARTUP;
                        USB_print("\n\rView All Records...");
                        serial_printAllRecords();
                    break;
                    case S_MAIN_FORMAT:
                        USB_print("\n\rWARNING: FORMATTING SD CARD");
                        USB_print("\n\rDo you really want to format the SD card?");
                        USB_print("\n\rTo cancel, type n and press enter");
                        USB_print("\n\rType \"yes, delete all records\" to continue: ");
                        s_main_state = S_FORMAT_SD;
                    break;
                    default:
                        {
                        USB_print("\n\rGot Command: ");
                        char to_send[5];
                        sprintf(to_send, "%d %s", cmd, last_serial_cmd);
                        USB_print(to_send);
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
                    // clear the current record
                    new_record = (Record){"","","",false,""};
                    USB_print("\n\rEnter Record Title: ");
                    s_sec_state = ADD_REC_TITLE;
                break;
                case ADD_REC_TITLE:
                    // once something has been entered
                    if (last_serial_cmd[0] != 0) {
                        // update curr record with title
                        strcpy(new_record.title, last_serial_cmd);
                        clr_last_serial_cmd();
                        // set up for username entry
                        USB_print("\n\rEnter username for ");
                        USB_print(new_record.title);
                        USB_print(": ");
                        s_sec_state = ADD_REC_UNAME;
                    }
                break;
                case ADD_REC_UNAME:
                    // once a title has been entered
                    if (last_serial_cmd[0] != 0) {
                        // update curr record with username
                        strcpy(new_record.username, last_serial_cmd);
                        clr_last_serial_cmd();
                        // set up for pw entry
                        USB_print("\n\rPassword for user ");
                        USB_print(new_record.username);
                        USB_print(": ");
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
                                USB_print("\n\rEnter Password for user ");
                                USB_print(new_record.username);
                                USB_print(": ");
                                s_sec_state = ADD_REC_PASS_ENTRY;
                            break;
                            case PASS_MENU_GEN:
                                {
                                USB_print("\n\rGenerating password! ");
                                
                                // generate password
                                char pass[17];
                                generate_password(pass);
                                strcpy(new_record.password, pass);
                                USB_print("\n\rGenerated Password for user ");
                                USB_print(new_record.username);
                                USB_print(": ");
                                USB_print(new_record.password);
                                // ask user if they want a TOTP code to be added
                                USB_print("\n\r Would you like to enter a TOTP key? y/n: ");
                                s_sec_state = ADD_REC_PROMPT_TOTP;
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
                        strcpy(new_record.password, last_serial_cmd);
                        clr_last_serial_cmd();
                        // show password to user
                        USB_print("\n\rSet password for user ");
                        USB_print(new_record.username);
                        USB_print(": ");
                        USB_print(new_record.password);
                        // ask user if they want a TOTP code to be added
                        USB_print("\n\r Would you like to enter a TOTP key? y/n: ");
                        s_sec_state = ADD_REC_PROMPT_TOTP;
                    }
                break;
                case ADD_REC_PROMPT_TOTP:
                    // check whether the user wants to input totp key
                    if (last_serial_cmd[0] != 0) {
                        if (last_serial_cmd[0] == 'y') {
                            // ask user to enter their totp key
                            USB_print("\n\rEnter TOTP Key (20 bytes, base32 encoded)");
                            USB_print("\n\rNote: Only 6-digit TOTP generation currently supported");
                            USB_print("\n\r> ");
                            s_sec_state = ADD_REC_TOTP_ENTRY;
                        } else {
                            USB_print("\n\rSkipping TOTP Entry");
                            new_record.totp_enabled = false;
                            s_sec_state = ADD_REC_CONF_PROMPT;
                        }
                        clr_last_serial_cmd(); // clear command buffer
                    }
                break;
                case ADD_REC_TOTP_ENTRY:
                    // wait for the user to finish entering their totp code
                    // and save it in the new_record
                    if (last_serial_cmd[0] != 0) {
                        // convert base32 ascii input to byte array
                        // we use google authenticator base32 code for this
                        // and store the key directly into the new record
                        base32_decode((uint8_t*) last_serial_cmd, new_record.totp_key, 20);
                        new_record.totp_enabled = true; // enable totp for this record
                        // re encode to confirm correctness and echo to user
                        // we just reuse last_serial_cmd as a buffer to avoid allocating 40 bytes
                        base32_encode(new_record.totp_key, 20, last_serial_cmd, last_serial_cmd_length);
                        USB_print("\n\rRecieved TOTP Key: ");
                        USB_print(last_serial_cmd);
                        s_sec_state = ADD_REC_CONF_PROMPT; // next: ask user to confirm
                        clr_last_serial_cmd();
                    }
                break;
                case ADD_REC_CONF_PROMPT:
                    // echo full record and prompt user to confirm
                    USB_print("\n\rFull Record to Add:");
                    serial_print_record(new_record);
                    USB_print("\n\r\n\rEnter 'y' to confirm, anything else to cancel.");
                    s_sec_state = ADD_REC_CONFIRM;
                break;
                case ADD_REC_CONFIRM:
                    // handle user confirmation input
                    if (last_serial_cmd[0] != 0) {
                        if (last_serial_cmd[0] == 'y') {
                            // new_record should be complete
                            // save the new record to sd
                            SD_addRecord(&new_record);
                            // tell ui to reload records
                            serial_status = S_STATUS_REQUIRE_RELOAD;
                        } else {
                            USB_print("\n\rCancelling!");
                        }
                        clr_last_serial_cmd();
                        s_main_state = S_MAIN_MENU;
                        serial_print_menu(main_menu);
                    }
                break;
            }
        break;
        case S_FORMAT_SD:
            // check if user inputted "yes, delete all records"
            if (last_serial_cmd[0] != 0) {
                if (!strcmp(last_serial_cmd, "yes, delete all records")) {
                    // this will format the SD card and recreate the db file
                    USB_print("\n\rFormatting SD... ");
                    SD_createDB(false);
                    USB_print("SD card formatted successfully.");
                    s_main_state = S_STARTUP;
                } else {
                    // that command was not entered, cancel
                    USB_print("\n\rCancelling, not modifying records");
                    s_main_state = S_STARTUP;
                }
            }
        break;
    }
}


/* [] END OF FILE */
