/* 
    Defines functions for 
    interfacing with the 
    sd-card database
*/
    
#ifndef SD_RECORDDB
#define SD_RECORDDB

#include <FS.h>
#include <stdbool.h>
#include <pk_record.h>
    
#include <stdio.h>

char sdFile[9];
char sdVolName[10];
FS_FILE * pFile;        // hold current file pointer
    
void SD_createDB(bool write_example);
bool SD_readRecords(int rec_index, int num_records, Record * out);
bool SD_addRecord(Record * to_add);
int SD_getNumRecords();


#endif

/* [] END OF FILE */
