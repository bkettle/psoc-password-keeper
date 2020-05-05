/*
    functions for the
    keyboard emulation
    and usb-uart
*/
    
#ifndef PK_USB
#define PK_USB
    
#include <project.h>
#include <stdbool.h>
#include <ssd1306.h>

// USB
void USB_handleInput(char * cmd_output);

void USB_print(char * str_to_print);

bool USB_isReady();
    
/********************************************************
*     KEYBOARD EMULATION STUFF                          *
********************************************************/
    
enum
{
    EP_KEYBOARD_IN = 4,
    EP_KEYBOARD_OUT = 5
};

bool KB_Initialized;
int8 KB_Data[8]; // [0]: Modifiers [1]: reserved [2:7]: Key codes
typedef struct {
    int uid;
    bool uppercase;
} hid_usage;

// this will start at ascii 32 to get to keyboard values
hid_usage ascii_to_hid[96];

void KB_TypeChar(hid_usage hid_id);

void ascii_test();

void USB_Initialize();

void KB_TypeString(char* str_to_type, bool draw);

#endif

/* [] END OF FILE */
