/********************************************************
*     KEYBOARD EMULATION & USB_UART STUFF               *
********************************************************/

#include <pk_usb.h>

// holds command in progress
char rx_buffer[40] = {0};
int rx_buff_length = 40;

// hold current USB input
// has to be long to support copy/paste
char inputbuffer[40];

void clr_rx_buffer(){ 
    memset(rx_buffer, 0, 40); 
    rx_buff_length = 0;
};

void USB_handleInput(char * cmd_output) {
    // updates the buffer given based on the 
    // input recieved
    if (USBFS_DataIsReady()) {
        while (USBFS_DataIsReady()) {
            // this only works as intended for 1 character at a time,
            // could add a loop over inputbuffer to fix this
            int count = USBFS_GetAll((uint8_t*) inputbuffer);
            inputbuffer[count] = 0; // null terminate the input
            if (inputbuffer[0] == 0xD) {
                // move buffer into cmd_output on enter
                strcpy(cmd_output, rx_buffer);
                clr_rx_buffer();
            } else if (inputbuffer[0] == 0x7F) {
                // handle backspaces
                int rx_len = strlen(rx_buffer);
                if (rx_len > 0) {
                    // if there is something in rx_buffer
                    // then replace the last thing with \0
                    rx_buffer[rx_len - 1] = 0;
                }
                // echo the backspace to the terminal
                USBFS_PutChar(0x7F);
            } else {
                USB_print(inputbuffer);    // echo recieved character
                // add non-enter characters to rx buffer
                strcat(rx_buffer, inputbuffer);           
            } 
        }
    }    
}
///////// USB CDC
void USB_print(char * str_to_print) {
    // wait until computer is ready
    // then send.
    while (!USBFS_CDCIsReady());
    USBFS_PutString(str_to_print);
}

bool USB_isReady() {
    return USBFS_CDCIsReady();
}



///////// KEYBOARD

// initialize keyboard variables
bool KB_Initialized = false;
int8 KB_Data[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // [0]: Modifiers [1]: reserved [2:7]: Key codes

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

void USB_Initialize() {
    // usb keyboard setup
    // exit if we already intialized the keyboard
    if (KB_Initialized) return;
    display_clear();
    gfx_setCursor(20,20);
    gfx_println("initializing usb...");
    gfx_println("must be connected to micro-usb port");
    display_update();
    CyGlobalIntEnable; /* Enable global interrupts. */
    USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);   /* Start USBFS Operation/device 0 and with 5V operation */ 	
    LED_Write(1);
	while(!USBFS_bGetConfiguration()) {      		/* Wait for Device to enumerate */
        // should add a timeout here
    }
    LED_Write(0);
    USBFS_LoadInEP(EP_KEYBOARD_IN, (uint8 *)KB_Data, 8); 	/* Loads an inital value into EP1 and sends it out to the PC */
    USBFS_CDC_Init(); // initialize cdc comms
    KB_Initialized = true;
}

void KB_TypeString(char* str_to_type, bool draw) {
    // enters the given string through keyboard emulation
    // takes a li bit of time
    USB_Initialize();
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
        //USB_print(str_to_type);
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

/* [] END OF FILE */
