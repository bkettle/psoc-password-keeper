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

int counter = 0;
int curr_a;
int prev_a;

// led1 should be OFF for ccw, ON for CW
// inspired by this https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/

// that's great, but I should use this https://www.cypress.com/documentation/component-datasheets/quadrature-decoder-quaddec

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
        curr_a = enc_a_Read();
        
        if (curr_a != prev_a) {
            if (enc_b_Read() != curr_a) {
                // clockwise
                LED1_Write(0);
                counter += 1;
            } else {
                // counterclockwise
                counter -= 1;
                LED1_Write(1);
            }
        }
        prev_a = curr_a;
        
    }
}

/* [] END OF FILE */
