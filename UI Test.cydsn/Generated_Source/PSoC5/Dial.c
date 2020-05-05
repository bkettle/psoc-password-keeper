/*******************************************************************************
* File Name: Dial.c  
* Version 3.0
*
* Description:
*  This file provides the source code to the API for the Quadrature Decoder
*  component.
*
* Note:
*  None.
*   
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Dial.h"

#if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
    #include "Dial_PVT.h"
#endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT */

uint8 Dial_initVar = 0u;


/*******************************************************************************
* Function Name: Dial_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default QuadDec configuration provided with customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Dial_Init(void) 
{
    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
        /* Disable Interrupt. */
        CyIntDisable(Dial_ISR_NUMBER);
        /* Set the ISR to point to the Dial_isr Interrupt. */
        (void) CyIntSetVector(Dial_ISR_NUMBER, & Dial_ISR);
        /* Set the priority. */
        CyIntSetPriority(Dial_ISR_NUMBER, Dial_ISR_PRIORITY);
    #endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: Dial_Enable
********************************************************************************
*
* Summary:
*  This function enable interrupts from Component and also enable Component's
*  ISR in case of 32-bit counter.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Dial_Enable(void) 
{
    uint8 enableInterrupts;

    Dial_SetInterruptMask(Dial_INIT_INT_MASK);

    /* Clear pending interrupts. */
    (void) Dial_GetEvents();
    
    enableInterrupts = CyEnterCriticalSection();

    /* Enable interrupts from Statusi register */
    Dial_SR_AUX_CONTROL |= Dial_INTERRUPTS_ENABLE;

    CyExitCriticalSection(enableInterrupts);        

    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
        /* Enable Component interrupts */
        CyIntEnable(Dial_ISR_NUMBER);
    #endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: Dial_Start
********************************************************************************
*
* Summary:
*  Initializes UDBs and other relevant hardware.
*  Resets counter, enables or disables all relevant interrupts.
*  Starts monitoring the inputs and counting.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  Dial_initVar - used to check initial configuration, modified on
*  first function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Dial_Start(void) 
{
    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT)
        Dial_Cnt8_Start();
        Dial_Cnt8_WriteCounter(Dial_COUNTER_INIT_VALUE);
    #else
        /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT) || 
        *  (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT) 
        */
        Dial_Cnt16_Start();
        Dial_Cnt16_WriteCounter(Dial_COUNTER_INIT_VALUE);
    #endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT */
    
    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)        
       Dial_count32SoftPart = 0;
    #endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT */

    if (Dial_initVar == 0u)
    {
        Dial_Init();
        Dial_initVar = 1u;
    }

    Dial_Enable();
}


/*******************************************************************************
* Function Name: Dial_Stop
********************************************************************************
*
* Summary:
*  Turns off UDBs and other relevant hardware.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Dial_Stop(void) 
{
    uint8 enableInterrupts;

    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT)
        Dial_Cnt8_Stop();
    #else 
        /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT) ||
        *  (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
        */
        Dial_Cnt16_Stop();    /* counter disable */
    #endif /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT) */
 
    enableInterrupts = CyEnterCriticalSection();

    /* Disable interrupts interrupts from Statusi register */
    Dial_SR_AUX_CONTROL &= (uint8) (~Dial_INTERRUPTS_ENABLE);

    CyExitCriticalSection(enableInterrupts);

    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
        CyIntDisable(Dial_ISR_NUMBER);    /* interrupt disable */
    #endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: Dial_GetCounter
********************************************************************************
*
* Summary:
*  Reports the current value of the counter.
*
* Parameters:
*  None.
*
* Return:
*  The counter value. Return type is signed and per the counter size setting.
*  A positive value indicates clockwise movement (B before A).
*
* Global variables:
*  Dial_count32SoftPart - used to get hi 16 bit for current value
*  of the 32-bit counter, when Counter size equal 32-bit.
*
*******************************************************************************/
int8 Dial_GetCounter(void) 
{
    int8 count;
    uint8 tmpCnt;

    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
        int16 hwCount;

        CyIntDisable(Dial_ISR_NUMBER);

        tmpCnt = Dial_Cnt16_ReadCounter();
        hwCount = (int16) ((int32) tmpCnt - (int32) Dial_COUNTER_INIT_VALUE);
        count = Dial_count32SoftPart + hwCount;

        CyIntEnable(Dial_ISR_NUMBER);
    #else 
        /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT) || 
        *  (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT)
        */
        #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT)
            tmpCnt = Dial_Cnt8_ReadCounter();
        #else /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT) */
            tmpCnt = Dial_Cnt16_ReadCounter();
        #endif  /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT */

        count = (int8) ((int16) tmpCnt -
                (int16) Dial_COUNTER_INIT_VALUE);

    #endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT */ 

    return (count);
}


/*******************************************************************************
* Function Name: Dial_SetCounter
********************************************************************************
*
* Summary:
*  Sets the current value of the counter.
*
* Parameters:
*  value:  The new value. Parameter type is signed and per the counter size
*  setting.
*
* Return:
*  None.
*
* Global variables:
*  Dial_count32SoftPart - modified to set hi 16 bit for current
*  value of the 32-bit counter, when Counter size equal 32-bit.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Dial_SetCounter(int8 value) 
{
    #if ((Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT) || \
         (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT))
        uint8 count;
        
        if (value >= 0)
        {
            count = (uint8) value + Dial_COUNTER_INIT_VALUE;
        }
        else
        {
            count = Dial_COUNTER_INIT_VALUE - (uint8)(-value);
        }
        #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT)
            Dial_Cnt8_WriteCounter(count);
        #else /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT) */
            Dial_Cnt16_WriteCounter(count);
        #endif  /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT */
    #else /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT) */
        CyIntDisable(Dial_ISR_NUMBER);

        Dial_Cnt16_WriteCounter(Dial_COUNTER_INIT_VALUE);
        Dial_count32SoftPart = value;

        CyIntEnable(Dial_ISR_NUMBER);
    #endif  /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT) ||
             * (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT)
             */
}


/*******************************************************************************
* Function Name: Dial_GetEvents
********************************************************************************
* 
* Summary:
*   Reports the current status of events. This function clears the bits of the 
*   status register.
*
* Parameters:
*  None.
*
* Return:
*  The events, as bits in an unsigned 8-bit value:
*    Bit      Description
*     0        Counter overflow.
*     1        Counter underflow.
*     2        Counter reset due to index, if index input is used.
*     3        Invalid A, B inputs state transition.
*
*******************************************************************************/
uint8 Dial_GetEvents(void) 
{
    return (Dial_STATUS_REG & Dial_INIT_INT_MASK);
}


/*******************************************************************************
* Function Name: Dial_SetInterruptMask
********************************************************************************
*
* Summary:
*  Enables / disables interrupts due to the events.
*  For the 32-bit counter, the overflow, underflow and reset interrupts cannot
*  be disabled, these bits are ignored.
*
* Parameters:
*  mask: Enable / disable bits in an 8-bit value, where 1 enables the interrupt.
*
* Return:
*  None.
*
*******************************************************************************/
void Dial_SetInterruptMask(uint8 mask) 
{
    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
        /* Underflow, Overflow and Reset interrupts for 32-bit Counter are always enable */
        mask |= (Dial_COUNTER_OVERFLOW | Dial_COUNTER_UNDERFLOW |
                 Dial_COUNTER_RESET);
    #endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT */

    Dial_STATUS_MASK = mask;
}


/*******************************************************************************
* Function Name: Dial_GetInterruptMask
********************************************************************************
*
* Summary:
*  Reports the current interrupt mask settings.
*
* Parameters:
*  None.
*
* Return:
*  Enable / disable bits in an 8-bit value, where 1 enables the interrupt.
*  For the 32-bit counter, the overflow, underflow and reset enable bits are
*  always set.
*
*******************************************************************************/
uint8 Dial_GetInterruptMask(void) 
{
    return (Dial_STATUS_MASK & Dial_INIT_INT_MASK);
}


/* [] END OF FILE */
