// OTP implementation using HMAC
// for a little endian system

#ifndef OTP_H
#define OTP_H
    
// counter can be any integer, will be converted to 8 bytes
int calc_hotp(int counter, char * key, int key_len);

int calc_totp(int unixtime, char* key, int key_len, int timestep, int T0);

#endif
