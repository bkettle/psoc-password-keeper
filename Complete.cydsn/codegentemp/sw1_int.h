/*******************************************************************************
* File Name: sw1_int.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_sw1_int_H)
#define CY_ISR_sw1_int_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void sw1_int_Start(void);
void sw1_int_StartEx(cyisraddress address);
void sw1_int_Stop(void);

CY_ISR_PROTO(sw1_int_Interrupt);

void sw1_int_SetVector(cyisraddress address);
cyisraddress sw1_int_GetVector(void);

void sw1_int_SetPriority(uint8 priority);
uint8 sw1_int_GetPriority(void);

void sw1_int_Enable(void);
uint8 sw1_int_GetState(void);
void sw1_int_Disable(void);

void sw1_int_SetPending(void);
void sw1_int_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the sw1_int ISR. */
#define sw1_int_INTC_VECTOR            ((reg32 *) sw1_int__INTC_VECT)

/* Address of the sw1_int ISR priority. */
#define sw1_int_INTC_PRIOR             ((reg8 *) sw1_int__INTC_PRIOR_REG)

/* Priority of the sw1_int interrupt. */
#define sw1_int_INTC_PRIOR_NUMBER      sw1_int__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable sw1_int interrupt. */
#define sw1_int_INTC_SET_EN            ((reg32 *) sw1_int__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the sw1_int interrupt. */
#define sw1_int_INTC_CLR_EN            ((reg32 *) sw1_int__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the sw1_int interrupt state to pending. */
#define sw1_int_INTC_SET_PD            ((reg32 *) sw1_int__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the sw1_int interrupt. */
#define sw1_int_INTC_CLR_PD            ((reg32 *) sw1_int__INTC_CLR_PD_REG)


#endif /* CY_ISR_sw1_int_H */


/* [] END OF FILE */
