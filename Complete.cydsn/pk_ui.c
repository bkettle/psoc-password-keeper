/* 
    holds functions 
    for the OLED UI
*/

#include <pk_ui.h>

uint8 display_state = STARTUP;

/***************************************************
*       VALUES FOR RECORD DETAIL MENU              *
***************************************************/
// menu items for record detail menu
int detail_menu_size = 5;
char detail_menu_items[5][20] = {
    "type username",
    "type password",
    "view credentials",
    "view TOTP code",
    "<- go back"
};
// menu coordinates
int detail_menu_top = 20;
int detail_menu_left = 20;

// load first items from memory
int buff_offset = 0; // sd card location of first record
int buff_size = 12;
int num_loaded = 0;
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

// store the previous index to calculate if a change occurred
int prev_index = 0;

// DEBUG
void debug_printBuff() {
    UART_1_PutString("\n\r------------");
    char to_print[10];
    sprintf(to_print, "%d", SD_getNumRecords());
    UART_1_PutString(to_print);
    for (int i=0; i<num_loaded; i++) {
        serial_print_record(record_buffer[i]);
    }
}
    
// update the buffer from the sd card
// loads 5 behind current, then current, then 6
// sets buff_offset and num_loaded
void ui_updateBuffer(int curr_index) {
    // determine where the new buffer should start
    buff_offset = 0;
    // check if there is enough space for 5 behind
    if (curr_index - 5 > 0) 
        buff_offset = curr_index-5;
    
    // find how many records we should load
    // based on the number in the SD card
    int sd_size = SD_getNumRecords();
    num_loaded = sd_size;
    // if there are more total records than
    // will fit in the buffer
    if (buff_offset + buff_size < sd_size)
        num_loaded = buff_size;
    
    SD_readRecords(buff_offset, num_loaded, record_buffer);
    num_loaded = num_loaded;
    debug_printBuff();
}
    
    
// UI Methods
void ui_drawRecordList(Record * buff, int curr_index) {
    /* uses the record buff, the current record number,
        and the current buffer offset to print the current record menu */
    int buff_index = curr_index - buff_offset;
    // handle overflow beyond bounds
    if (buff_index < 0) {
        Dial_SetCounter(0);
        buff_index = 0;
    } else if (buff_index > num_loaded -1) {
        // reached end of records
        // loaded in the buffer
        // don't move further obvs
        // if we're here, there must not be any more
        // records in the sd card (i hope)
        // because that should be taken care of at the end
        // of this function (centering the current rec)
        Dial_SetCounter(num_loaded - 1);
        buff_index = num_loaded - 1;
    }
    display_clear();
    gfx_fillRect(0, 22, 100, 20, WHITE);
    gfx_setTextSize(1);
    gfx_setCursor(0,6);
    gfx_setTextColor(WHITE);
    gfx_setTextSize(1);
    // only print these if we're not near the bottom end, otherwise "list end"
    // buffer is always loaded from the bottom, so we can compare to 1,2 directly
    gfx_println(buff_index >= 2 ? buff[buff_index-2].title : "list end"); // -2 line
    gfx_println(buff_index >= 1 ? buff[buff_index-1].title : "list end"); // -1 line
    gfx_setTextSize(2);
    gfx_setTextColor(BLACK);
    gfx_setCursor(5,24);
    gfx_println(buff[buff_index].title); // 0 line
    gfx_setTextSize(1);
    gfx_setTextColor(WHITE);
    gfx_setCursor(0,42);
    // only print these if we're not near the top end, otherwise "list end"
    gfx_println(buff_index <= num_loaded-2 ? buff[buff_index+1].title : "list end"); // +1 line
    gfx_println(buff_index <= num_loaded-3 ? buff[buff_index+2].title : "list end"); // +2 line
    display_update();
    
    // BUFFER CENTERING
    // update record buffer if approaching the end
    // and we have a different cursor value than last time
    if (curr_index != prev_index) {
        if (curr_index - buff_offset < 2) {
            // currently highlighted is the second from the bottom
            // we want this to be middle-ish
        ui_updateBuffer(curr_index);
        } else if (curr_index - buff_offset > buff_size - 3) {
            // second from top is currently hightlighted
            // so center it
            ui_updateBuffer(curr_index);
        }
    }
}

void ui_recordDetailUpdate(int cursor) {
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

void ui_drawRecordDetail(Record record) {
    display_clear();
    gfx_setCursor(0,6);
    gfx_setTextSize(1);
    gfx_setTextColor(WHITE);
    gfx_println(record.title);
    for (int i = 0; i<detail_menu_size; i++) {
        gfx_setCursor(detail_menu_left, detail_menu_top + 9*i);
        gfx_println(detail_menu_items[i]);
    }
    display_update();
    Dial_SetCounter(0);
    ui_recordDetailUpdate(Dial_GetCounter());
}

void ui_fsm(bool * enc_sw_flag) {
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
            // set the initial buffer up
            ui_updateBuffer(Dial_GetCounter());
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
            if (*enc_sw_flag == true) {
                display_state = RECORD_SELECT;
                Dial_SetCounter(0);
                ui_drawRecordList(record_buffer, Dial_GetCounter());
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
                ui_drawRecordList(record_buffer, Dial_GetCounter());
                curr_record = record_buffer[Dial_GetCounter() - buff_offset];
            };
            if (*enc_sw_flag) {
                display_state = RECORD_DETAIL;
                ui_drawRecordDetail(curr_record);
            }
        break;
        case RECORD_DETAIL:
            ui_recordDetailUpdate(Dial_GetCounter());
            if (*enc_sw_flag) {
                switch (Dial_GetCounter()) {
                    case RECORD_TYPE_USER: 
                    {
                        KB_TypeString(curr_record.username, true);
                        // typestring writes to screen, so redraw.
                        ui_drawRecordDetail(curr_record);
                    }
                    break;
                    case RECORD_TYPE_PW:
                    {
                        KB_TypeString(curr_record.password, true);
                        // typestring writes to screen, so redraw.
                        ui_drawRecordDetail(curr_record);
                    }
                    break;
                    case RECORD_VIEW_CREDENTIALS:
                    break;
                    case RECORD_VIEW_TOTP:
                    break;
                    case RECORD_BACK:
                        // just go back to the record menu
                        display_state = RECORD_SELECT;
                        Dial_SetCounter(0);
                        ui_updateBuffer(0);
                        ui_drawRecordList(record_buffer, Dial_GetCounter());
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
    prev_index = Dial_GetCounter();
}

/* [] END OF FILE */
