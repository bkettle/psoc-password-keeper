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
#include "sha1.h"
#include "hmac.h"
#include "otp.h"

#include <stdio.h>

// print hex of a char array
void phex(unsigned char * arr, size_t size) {
    char tp[3]; //holds things to be printed
    for (uint i=0; i<size; i++) {
        sprintf(tp, "%X", arr[i]);
        UART_1_PutString(tp);
    }
    UART_1_PutString(" ");
}

// I THINK THIS WORKS NOW
//void verbose_hotp() {
//    // HOTP
//    // right now only generates 6-digit ones but could do more
//    UART_1_PutString("\n\rHOTP FUNCTION: ");
//    uint64_t c_int = (uint64_t) counter; // convert counter to 8 Bytes
//    c_int = __builtin_bswap64(c_int); // convert counter to big-endian
//    unsigned char * c = &c_int; // get pointer to char array for thats
//    
//    
// 
//    UART_1_PutString("\n\r Count: 0x");
//    phex(c, 8);
//    UART_1_PutString(" Key: 0x");
//    phex(key, key_len);
//    unsigned char result[20];
//    uint rlen = 20;
//    hmac_sha1(key, key_len, c, 8, result, &rlen);
//    UART_1_PutString("\n\rHMAC Calculated: ");
//    phex(result, 20);
//    
//    // CALCULATE OFFSET
//    char tp[3];
//    sprintf(tp, " -> %X ", result[19]);
//    UART_1_PutString(tp);
//    // just take lower 4 bits of the last byte
//    uint8_t offset = (uint8_t) result[19] & 0xF;
//    sprintf(tp, "%X \n\r", offset);
//    UART_1_PutString(tp);
//    
//    // TRUNCATE
//    // take all but msb of result[offset]...result[offset+3]
//    char trunc[4];
//    UART_1_PutString("truncating: ");
//    strncpy(trunc, (char*) result+offset, 4);
//    phex(trunc, 4);
//    // endianness determines whether this is trunc[0] or [3]
//    // this is little endian, so trunc[3] is msB I think
//    // delete most significant bit, could do this by anding with 0111111b too
//    trunc[0] = trunc[0] & 0x7F;
//    phex(trunc, 4);
//    UART_1_PutString("\n\r");
//    
//    // compute hotp value
//    UART_1_PutString("hotp value: ");
//    // psoc little-endian, so we need to flip the order of bytes
//    // because algorithm treats it as big-endian
//    // nevermind that was messing it up!
//    // we just assign them as expected
//    uint32_t t_num = trunc[0] << 24 | trunc[1] << 16 | trunc[2] << 8 | trunc[3];
//    char to_print[20];
//    sprintf(to_print, "%d ", t_num);
//    UART_1_PutString(to_print);
//    // mod with 10^6 for a 6-digit number
//    uint32_t hotp_val = t_num % 1000000;
//    sprintf(to_print, "-> %d", hotp_val);
//    UART_1_PutString(to_print);
//    UART_1_PutString("\n\r");
//}
    
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_1_Start();

    for(;;)
    {
        /* Place your application code here. */
        
        // SHA TEST
        SHA1_CTX sha; 
        uint8_t results[20]; 
        char *buf; 
        int n;

        buf = ""; 
        n = strlen(buf); 
        SHA1Init(&sha); 
        SHA1Update(&sha, (uint8_t *)buf, n); 
        SHA1Final(results, &sha);
        UART_1_PutString("\n\rSHA TEST 1: ");
        phex(results, 20);
        
        
        // HMAC TEST 1
        unsigned char sec_key1[] = "Jefe";
        unsigned char data1[] = "what do ya want for nothing?";
        unsigned char out1[256] = {0};
        uint len1 = sizeof(out1);
        hmac_sha1(sec_key1, 4, data1, strlen(data1), out1, &len1);
        UART_1_PutString("\n\rHMAC TEST 1: ");
        phex(out1, 20);
        UART_1_PutString("\n\r");
        
        // HMAC TEST 2
        unsigned char sec_key2[] = {0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
                                    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c};
        unsigned char data2[] = "Test With Truncation";
        unsigned char out2[256] = {0};
        uint len2 = sizeof(out2);
        hmac_sha1(sec_key2, 20, data2, strlen(data2), out2, &len2);
        UART_1_PutString("HMAC TEST 2: ");
        phex(out2, 20);
        UART_1_PutString("\n\r");
        
        // HMAC TEST 3
        unsigned char sec_key3[80];
        memset(sec_key3, 0xaa, 80);
        unsigned char data3[] = "Test Using Larger Than Block-Size Key - Hash Key First";
        unsigned char out3[256] = {0};
        uint len3 = sizeof(out3);
        hmac_sha1(sec_key3, 80, data3, 54, out3, &len3);
        UART_1_PutString("HMAC TEST 3: ");
        phex(out3, 20);
        UART_1_PutString("\n\r");
        
        // HMAC TEST 4
        unsigned char sec_key4[80];
        memset(sec_key4, 0xaa, 80);
        unsigned char data4[] = "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data";
        unsigned char out4[256] = {0};
        uint len4 = sizeof(out4);
        hmac_sha1(sec_key4, 80, data4, 73, out4, &len4);
        UART_1_PutString("HMAC TEST 4: ");
        phex(out4, 20);
        UART_1_PutString("\n\r");
        
        
        
        
//      // HOTP
        UART_1_PutString("\n\r HOTP TEST 1:");
        unsigned char c[8] = {0};
        c[7] = 0x9; // set counter value to 1
        UART_1_PutString("\n\r Count: 0x");
        phex(c, 8);
        unsigned char key[] = "12345678901234567890";
        UART_1_PutString(" Key: 0x");
        phex(key, 20);
        unsigned char result[20];
        uint rlen = sizeof(result);
        hmac_sha1(key, 20, c, 8, result, &rlen);
        UART_1_PutString("\n\rHMAC Calculated: ");
        phex(result, 20);
//        
        // CALCULATE OFFSET
        char tp[3];
        sprintf(tp, " -> %X ", result[19]);
        UART_1_PutString(tp);
        // just take lower 4 bits of the last byte
        uint8_t offset = (uint8_t) result[19] & 0xF;
        sprintf(tp, "%X \n\r", offset);
        UART_1_PutString(tp);
        
        // TRUNCATE
        // take all but msb of result[offset]...result[offset+3]
        char trunc[4];
        UART_1_PutString("truncating: ");
        strncpy(trunc, (char*) result+offset, 4);
        phex(trunc, 4);
        // endianness determines whether this is trunc[0] or [3]
        // this is little endian, so trunc[3] is msB I think
        // delete most significant bit, could do this by anding with 0111111b too
        trunc[0] = trunc[0] & 0x7F;
        phex(trunc, 4);
        UART_1_PutString("\n\r");
        
        // compute hotp value
        UART_1_PutString("hotp value: ");
        // psoc little-endian, so we need to flip the order of bytes
        // because algorithm treats it as big-endian
        // nevermind that was messing it up!
        // we just assign them as expected
        uint32_t t_num = trunc[0] << 24 | trunc[1] << 16 | trunc[2] << 8 | trunc[3];
        char to_print[20];
        sprintf(to_print, "%d ", t_num);
        UART_1_PutString(to_print);
        // mod with 10^6 for a 6-digit number
        uint32_t hotp = t_num % 1000000;
        sprintf(to_print, "-> %d", hotp);
        UART_1_PutString(to_print);
        UART_1_PutString("\n\r");
        
//        calc_hotp(0, key, 20);
//        calc_hotp(1, key, 20);
//        calc_hotp(2, key, 20);
//        calc_hotp(3, key, 20);
//        calc_hotp(4, key, 20);
//        calc_hotp(5, key, 20);
//        calc_hotp(6, key, 20);
//        calc_hotp(7, key, 20);
        
        UART_1_PutString("\n\rTOTP Test: ");
        char totpkey[20] = "12345678901234567890";
        sprintf(to_print, "%d", calc_totp(59, totpkey, 20, 30, 0));
        UART_1_PutString(to_print);
        
        
        // quick endian test
        // shows that LEAST significant byte is at index 0
        uint16_t num = 1;
        UART_1_PutString("endian test: ");
        phex((unsigned char*) &num, 2);
        
        
        UART_1_PutString("------\n\r");
        CyDelay(5000);
    }
}

/* [] END OF FILE */
