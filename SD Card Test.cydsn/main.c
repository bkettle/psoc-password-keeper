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

char sdFile[9] = "pwdb";

// struct to hold each item, when decrypted they should fit this format
// 64 bytes total (for now)
// ALREADY IN OTHER FILE
typedef struct {
    char title[10];
    char username[35];
    char password[19];
} Record;
int record_size = 64; // record size in bytes
Record null_record = {"","",""};
Record example_record = {"aaaaaaaa", "aaaaaaaaaaaaaaaaa", "aaaaaaaaaaa"};


//////////// ALREADY IN SERIAL USB FILE
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



void SD_createDB(FS_FILE * pFile, char * sdVolName, bool write_example) {
    UART_1_PutString("\n\r SD card format");
    
    if(0 == FS_FormatSD(sdVolName)) {
        UART_1_PutString(" Succeeded");
    }
    else {
        UART_1_PutString(" Failed");
    }
    
    /* Create specific file for modification */
    pFile = FS_FOpen(sdFile, "w");
    
    /* Check if file was created */
    if(pFile) {
        /* Indicate successful file creation message */
        UART_1_PutString("\n\r File ");
        UART_1_PutString(sdFile);
        UART_1_PutString(" was opened");
        
        // write example record to the file
        if (write_example) {
            if(0 != FS_Write(pFile, &example_record, record_size)) {
                /* Inditate that data was written to a file */
                UART_1_PutString("\n\r\"0123456789\"");
                UART_1_PutString(sdFile);
                //LCD_Position(1u, 0u);
                UART_1_PutString(" written to file");
            }
            else {
                UART_1_PutString("\n\r Failed to write");
                //LCD_Position(1u, 0u);
                UART_1_PutString(" data to file");
            }
        }
        
        if(0 == FS_FClose(pFile)) {
            UART_1_PutString("\n\r File was closed");
        }
        else {
            UART_1_PutString("\n\r Failed to close");
            UART_1_PutString("file");
        }
    }
    else {
        UART_1_PutString("\n\r Failed to create");
        UART_1_PutString(" file");
    }
    
    UART_1_PutString("\n\r Format complete ");
}

bool SD_readRecords(FS_FILE * pFile, int rec_index, int num_records, Record * out) {
    /* reads records from card at the 
        specified index, and copies it to the location
        given by the out pointer.
    
        returns true if successful, false if not
    */
    // open the db file
    pFile = FS_FOpen(sdFile, "r");
    
    if (pFile) {
        // should first check that the file is long enough
        // to hold all the records the person asked for
        int db_size = FS_GetFileSize(pFile);
        if (db_size < (rec_index + num_records)*record_size) {
            // not enough records in db, abort
            UART_1_PutString("\n\r !! Requested records outside range!");
            return false;
        }
        
        
        // move the cursor to the start of the record we want
        // this record should be located at rec_index * record_size
        if (0 != FS_FSeek(pFile, rec_index*record_size, FS_SEEK_CUR)) {
            // seek failed, abort
            UART_1_PutString("\n\r !! Failed to move cursor!");
            return false;
        }
        
        FS_Read(pFile, out, record_size*num_records);
        
        if (0 != FS_FClose(pFile)) {
            // failed to close file
            UART_1_PutString("\n\r !! Failed to close file!");
            return false;
        }
    }
    
    return false;
}

bool SD_addRecord(FS_FILE * pFile, Record * to_add) {
    /* writes the record at the address given by to_add
        to the SD card at the end of the db.
    
        returns true if successful, false if not
    */
    // open the db file for APPENDING (a)
    pFile = FS_FOpen(sdFile, "a");
    
    if (pFile) {
        // cursor should be at the end of the file, bc appending
        
        // write the record in to_add to the file at the cursor pos
        // should be record_size long (I hope)
        FS_Write(pFile, to_add, record_size);
        
        if (0 != FS_FClose(pFile)) {
            // failed to close file
            UART_1_PutString("\n\r !! Failed to close file!");
            return false;
        }
    }
    
    return false;
}

int SD_getNumSavedRecords(FS_FILE * pFile) {
    /* returns the number of records
        currently saved in the sd card db
    */
    pFile = FS_FOpen(sdFile, "a");
    int db_size = 0;
    
    if (pFile) {
        db_size = FS_GetFileSize(pFile);
        
        if (0 != FS_FClose(pFile)) {
            // failed to close file
            UART_1_PutString("\n\r !! Failed to close file!");
            return false;
        }
        
    }
    return db_size;
}

int main() {
    char sdVolName[10];     /* Buffer that will hold SD card Volume name */
    U16 step = 1u;
    FS_FILE * pFile;
    
    /* Start LCD */
    //LCD_Start();
    UART_1_Start();
    
    /* Initialize file system */
    FS_Init();
    
    /* Get volume name of SD card #0 */
    if(0 != FS_GetVolumeName(0u, &sdVolName[0], 9u))
    {
        // display volume name
        UART_1_PutString("\n\r SD card name:");
        UART_1_PutString(sdVolName);
    }
    else
    {
        // getting volume name failed
        UART_1_PutString("\n\r Failed to get ");
        UART_1_PutString("SD card name");
    }
    
    // format the sd card and create the db
    //SD_createDB(pFile, sdVolName, true);
    //Record new_rec = {"bbb", "bbbb", "bbbbb"};
    //SD_addRecord(pFile, &new_rec);
    
    for(;;) {
        CyDelay(4000);
        Record curr_records[2];
        SD_readRecords(pFile, 0, 2, curr_records);
        serial_print_record(curr_records[0]);
        serial_print_record(curr_records[1]);
        
        UART_1_PutString("\n\r----");
        SD_readRecords(pFile, 1, 1, curr_records);
        serial_print_record(curr_records[0]);
        
    }
}

/* [] END OF FILE */
