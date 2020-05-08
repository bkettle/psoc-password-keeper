/* defines functions
    for interfacing with the user
    over serial.
*/

#ifndef PK_SERIALFUNCS
#define PK_SERIALFUNCS
#include <pk_record.h>
#include <project.h>
#include <ds3231.h>
#include <stdio.h>
#include <stdlib.h>

void serial_print_record(Record record);

typedef struct {
    int size;
    char title[20];
    char items[10][20];
} SerialMenu;

// MENU/Serial State DEFINITIONS
#define S_WAIT -1
#define S_STARTUP 0
#define S_MAIN_MENU 1
#define S_ADD_RECORD 2
#define S_FORMAT_SD 3

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
#define ADD_REC_CONFIRM 5

// command definitions for password menu
#define PASS_MENU_ENTER 1
#define PASS_MENU_GEN 2

// HOLDS THE RECORD BEING CREATED
Record new_record;

// print a menu and prompt user input
void serial_print_menu(SerialMenu menu);

// display a record prettily
void serial_print_record(Record record);

// invalid user input
void serial_inv_cmd();

// generate a password
void generate_password(char* out);

// for the rtc, just debugging stuff
void serial_print_dt(DateTime dt);

// more debug, print hex value of array
void serial_printHex(unsigned char * arr, size_t size);

CY_ISR(RX_INT);

CY_ISR(ENC_SW_INT);

// Serial state variables
int s_main_state;
int s_sec_state; // secondary state
void serial_fsm();
void serial_menu_init();


#endif

/* [] END OF FILE */
