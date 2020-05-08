// OTP implementation using HMAC
// for a little endian system

#include "otp.h"
#include "project.h"
#include "hmac.h"
#include <stdio.h>

// counter can be any integer, will be converted to 8 bytes
int calc_hotp(int counter, char * key, int key_len) {
    // I THINK THIS WORKS NOW
    // HOTP
    // right now only generates 6-digit ones but could do more
    //UART_1_PutString("\n\rHOTP FUNCTION: ");
    uint64_t c_int = (uint64_t) counter; // convert counter to 8 Bytes
    c_int = __builtin_bswap64(c_int); // convert counter to big-endian
    unsigned char * c = (unsigned char *) &c_int; // get pointer to char array for thats
    
    
 
    //UART_1_PutString("\n\r Count: 0x");
    //phex(c, 8);
    //UART_1_PutString(" Key: 0x");
    //phex(key, key_len);
    unsigned char result[20];
    uint rlen = 20;
    hmac_sha1((uint8_t *) key, key_len, c, 8, result, &rlen);
    //UART_1_PutString("\n\rHMAC Calculated: ");
    //phex(result, 20);
    
    // CALCULATE OFFSET
    //char tp[3];
    //sprintf(tp, " -> %X ", result[19]);
    //UART_1_PutString(tp);
    // just take lower 4 bits of the last byte
    uint8_t offset = (uint8_t) result[19] & 0xF;
    //sprintf(tp, "%X \n\r", offset);
    //UART_1_PutString(tp);
    
    // TRUNCATE
    // take all but msb of result[offset]...result[offset+3]
    char trunc[4];
    //UART_1_PutString("truncating: ");
    strncpy(trunc, (char*) result+offset, 4);
    //phex(trunc, 4);
    // endianness determines whether this is trunc[0] or [3]
    // this is little endian, so trunc[3] is msB I think
    // delete most significant bit, could do this by anding with 0111111b too
    trunc[0] = trunc[0] & 0x7F;
    //phex(trunc, 4);
    //UART_1_PutString("\n\r");
    
    // compute hotp value
    //UART_1_PutString("hotp value: ");
    // psoc little-endian, so we need to flip the order of bytes
    // because algorithm treats it as big-endian
    // nevermind that was messing it up!
    // we just assign them as expected
    uint32_t t_num = trunc[0] << 24 | trunc[1] << 16 | trunc[2] << 8 | trunc[3];
    char to_print[20];
    //sprintf(to_print, "%d ", t_num);
    //UART_1_PutString(to_print);
    // mod with 10^6 for a 6-digit number
    uint32_t hotp_val = t_num % 1000000;
    //sprintf(to_print, "-> %d", hotp_val);
    //UART_1_PutString(to_print);
    //UART_1_PutString("\n\r");
    
    return hotp_val;
}

int calc_totp(int unixtime, char* key, int key_len, int timestep, int T0) {
    // by default, timestep is 30 seconds and t0 is 0.
    int T = (unixtime - T0) / timestep;
    return calc_hotp(T, key, key_len);
}