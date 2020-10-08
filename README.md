# PSoC-based Hardware Password Manager

I created this project for a class at MIT, 6.115 Microcontroller Project Laboratory. Though the final project structure was modified in light of being sent home due to COVID-19, we were tasked with creaing a final project using the several Cypress PSoC development boards we were sent home with. I chose to create a password manager that was easy to carry with you, and did not rely on the cloud to transfer passwords between computers. 

## Overview

### Goals

* Remove the need to use a cloud-based password management service in order to have unique, secure passwords for every account.
* Avoid requiring the installation of external software, which is often platform-dependent and unable to be installed in work environments. 
* Create an intuitive user interface that requires minimal technical knowledge.

### Key Features

* Username and Password entry via HID Keyboard emulation.
* Record/Account Management via CDC serial device emulation.
* Support for entry and storage of TOTP 2FA secret keys, and generation of one-time passwords using the TOTP algorithm. 
* Account information encrypted with AES-128 using a password-derived key and stored on an SD card.

### Areas for Improvement

* Password-based key derivation is not terribly secure, based only on SHA-1 hashing. Could be improved with the implementation of an algorithm such as PBKDF2. though the limited computing power of the PSoC would limit effectiveness against an attack with a powerful computer. 

