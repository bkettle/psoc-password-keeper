/* 
    Defines functions for 
    interfacing with the 
    sd-card database
*/

#include <pk_sdrecorddb.h>

//Record example_record = {"example", "record", "this is an example"};

char sdFile[9] = "pwdb";

// ENCRYPTION
struct AES_ctx ctx;
uint8_t key[16] = {83, 13, 14, 15, 16, 17, 18, 19,
                    20, 245, 22, 23, 24, 25, 29, 27};
void enc_encryptRecord(Record * to_encrypt) {
    // encrypts the record
    // record size must be a mutliple of 16
    AES_init_ctx(&ctx, key);
    for (int i=0; i<(record_size/16); i++) {
        AES_ECB_encrypt(&ctx, (uint8_t *) to_encrypt + i*16);
    }
}

void enc_decryptRecord(Record * to_decrypt) {
    // decrypts the record
    AES_init_ctx(&ctx, key);
    for (int i=0; i<(record_size/16); i++) {
        AES_ECB_decrypt(&ctx, (uint8_t *) to_decrypt + i*16);
    }
}
// END ENCRYPTION


void SD_createDB(bool write_example) {
    UART_1_PutString("\n\r SD card format");
    
    char sdVolName[10];     /* Buffer that will hold SD card Volume name */
    
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
        
//        // write example record to the file
//        if (write_example) {
//            // this writes unencrypted record, will not work
//            if(0 != FS_Write(pFile, &example_record, record_size)) {
//                /* Inditate that data was written to a file */
//                UART_1_PutString("\n\r\"0123456789\"");
//                UART_1_PutString(sdFile);
//                //LCD_Position(1u, 0u);
//                UART_1_PutString(" written to file");
//            }
//            else {
//                UART_1_PutString("\n\r Failed to write");
//                //LCD_Position(1u, 0u);
//                UART_1_PutString(" data to file");
//            }
//        }
        
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

bool SD_readRecords(int rec_index, int num_records, Record * out) {
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
        
        // decrypt the records
        for (int i = 0; i<num_records; i++) {
            // find the address of each record and decrypt
            enc_decryptRecord(&out[i]);
        }
        
        if (0 != FS_FClose(pFile)) {
            // failed to close file
            UART_1_PutString("\n\r !! Failed to close file!");
            return false;
        }
    }
    
    return false;
}

bool SD_addRecord(Record * to_add) {
    /* writes the record at the address given by to_add
        to the SD card at the end of the db.
    
        returns true if successful, false if not
    */
    // open the db file for APPENDING (a)
    pFile = FS_FOpen(sdFile, "a");
    
    if (pFile) {
        // encrypt record
        enc_encryptRecord(to_add);
        
        // cursor should be at the end of the file, bc appending
        // write the record in to_add to the file at the cursor pos
        // should be record_size long (I hope)
        FS_Write(pFile, to_add, record_size);
        
        if (0 != FS_FClose(pFile)) {
            // failed to close file
            UART_1_PutString("\n\r !! Failed to close file!");
            return false;
        }
        UART_1_PutString("\n\r Record added successfully!");
    }
    
    return false;
}

int SD_getNumRecords() {
    pFile = FS_FOpen(sdFile, "r");
    
    int db_size = 0;
    
    if (pFile) {
        // should first check that the file is long enough
        // to hold all the records the person asked for
        db_size = FS_GetFileSize(pFile);
    }
    
    char to_print[10];
    sprintf(to_print, "%d", db_size);
    UART_1_PutString(to_print);
    return (db_size/record_size);
}

/* [] END OF FILE */
