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
#include <stdbool.h>

enum
{
    EP_KEYBOARD_IN = 4,
    EP_KEYBOARD_OUT = 5
};

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
    while(!USBFS_GetEPAckState(EP_KEYBOARD_IN));  			/* Wait for ACK before loading data */
    KB_Data[2] = hid_id.uid;
    KB_Data[0] = 2*(int)hid_id.uppercase;
    USBFS_LoadInEP(EP_KEYBOARD_IN, (uint8 *)KB_Data, 8); 	/* Load latest mouse data into EP1 and send to PC */
    
    // send a blank keystroke to clear the state of shift
    while(!USBFS_GetEPAckState(EP_KEYBOARD_IN));  			/* Wait for ACK before loading data */
    KB_Data[2] = 0;
    KB_Data[0] = 0;
    USBFS_LoadInEP(EP_KEYBOARD_IN, (uint8 *)KB_Data, 8); 	/* Load latest mouse data into EP1 and send to PC */
}

void ascii_test() {
    for (unsigned int i=0;i<95;i++){
        LED_Write(0);
        KB_TypeChar(ascii_to_hid[i]);
        LED_Write(1);
    }
}

static void USB_print(const char* s)
{
    while (!USBFS_CDCIsReady())
        ;
    USBFS_PutString(s);
}

void KB_TypeString(char* str_to_type) {
    for (unsigned int i = 0; i<strlen(str_to_type); i++) {
        // the ascii to hid table starts at ascii 32
        int shifted_ascii = (int) str_to_type[i] - 32;
        // type the current character
        KB_TypeChar(ascii_to_hid[shifted_ascii]);
    }
    while (!USBFS_CDCIsReady())
        ;
    USBFS_PutString(str_to_type);
}



int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);   /* Start USBFS Operation/device 0 and with 5V operation */ 	
    LED_Write(1);
	while(!USBFS_bGetConfiguration());      		/* Wait for Device to enumerate */
    LED_Write(0);
    USBFS_CDC_Init();
    USBFS_LoadInEP(EP_KEYBOARD_IN, (uint8 *)KB_Data, 8); 	/* Loads an inital value into EP1 and sends it out to the PC */
    
    char cmd_buff[20] = {0};
    
    LED_Write(0);
    CyDelay(100);
    LED_Write(1);
    CyDelay(100);
    LED_Write(0);
    CyDelay(100);
    LED_Write(1);
    CyDelay(100);
        
    for(;;)
    {
//        while(!USBFS_GetEPAckState(EP_KEYBOARD_IN));  			/* Wait for ACK before loading data */
//		USBFS_LoadInEP(EP_KEYBOARD_IN, (uint8 *)KB_Data, 8); 	/* Load latest mouse data into EP1 and send to PC */
//        
//        //KB_Data[2] = 0x04;
//        char to_write[] = "tomato ";
//        KB_TypeString(to_write);
//        //ascii_test();
//        
//        LED_Write(0);
//        CyDelay(1000);
//        LED_Write(1); 
//        CyDelay(1000);
        
        /* Handle the serial input. */

        if (USBFS_DataIsReady())
        {
            while (USBFS_DataIsReady())
            {
                char inputbuffer[64];
                int count = USBFS_GetAll((uint8_t*) inputbuffer);
                inputbuffer[count] = 0;
                USBFS_PutString(inputbuffer);    // echo recieved character
                // add non-enter characters to rx buffer
                strcat(cmd_buff, inputbuffer);
                USB_print("\n\r");
                USB_print(cmd_buff);
                USB_print("\n\r----------");
                
            }
            LED_Write(1);
            CyDelay(100);
            LED_Write(0);
        }
    }
    
    
}

/* [] END OF FILE */
