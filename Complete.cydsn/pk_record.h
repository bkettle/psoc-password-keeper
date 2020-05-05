/*
    Defines the Record datatype
    for use holding username,
    password, title data.
*/
    
#ifndef PK_RECORD
#define PK_RECORD

// struct to hold each item, when decrypted they should fit this format
// 64 bytes total (for now)
typedef struct {
    char title[10];
    char username[35];
    char password[19];
} Record;
int record_size; // record size in bytes

#endif

/* [] END OF FILE */
