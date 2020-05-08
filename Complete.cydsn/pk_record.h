/*
    Defines the Record datatype
    for use holding username,
    password, title data.
*/
    
#ifndef PK_RECORD
#define PK_RECORD

// struct to hold each item, when decrypted they should fit this format
// 64 bytes total (for now)
// not anymore, now 96 with totp
typedef struct {
    char title[10];
    char username[40];
    char password[26];
    char totp_key[20];
} Record;
int record_size; // record size in bytes

#endif

/* [] END OF FILE */
