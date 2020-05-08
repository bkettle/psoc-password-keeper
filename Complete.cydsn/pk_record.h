/*
    Defines the Record datatype
    for use holding username,
    password, title data.
*/
    
#ifndef PK_RECORD
#define PK_RECORD
    
#include <stdbool.h> // for totp_enabled

// struct to hold each item, when decrypted they should fit this format
// 64 bytes total (for now)
// not anymore, now 96 with totp
typedef struct {
    char title[10];
    char username[39];
    char password[26];
    bool totp_enabled; // could hold 8 settings in same space w uint8_t
    unsigned char totp_key[20];
} Record;
int record_size; // record size in bytes

#endif

/* [] END OF FILE */
