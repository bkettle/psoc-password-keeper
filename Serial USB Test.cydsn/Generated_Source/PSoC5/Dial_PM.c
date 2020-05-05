/*******************************************************************************
* File Name: Dial_PM.c
* Version 3.0
*
* Description:
*  This file contains the setup, control and status commands to support 
*  component operations in low power mode.  
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

static Dial_BACKUP_STRUCT Dial_backup = {0u};


/*******************************************************************************
* Function Name: Dial_SaveConfig
********************************************************************************
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Dial_SaveConfig(void) 
{
    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT)
        Dial_Cnt8_SaveConfig();
    #else 
        /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT) || 
         * (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
         */
        Dial_Cnt16_SaveConfig();
    #endif /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT) */
}


/*******************************************************************************
* Function Name: Dial_RestoreConfig
********************************************************************************
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Dial_RestoreConfig(void) 
{
    #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT)
        Dial_Cnt8_RestoreConfig();
    #else 
        /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT) || 
         * (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT) 
         */
        Dial_Cnt16_RestoreConfig();
    #endif /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT) */
}


/*******************************************************************************
* Function Name: Dial_Sleep
********************************************************************************
* 
* Summary:
*  Prepare Quadrature Decoder Component goes to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Dial_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Dial_Sleep(void) 
{
    if (0u != (Dial_SR_AUX_CONTROL & Dial_INTERRUPTS_ENABLE))
    {
        Dial_backup.enableState = 1u;
    }
    else /* The Quadrature Decoder Component is disabled */
    {
        Dial_backup.enableState = 0u;
    }

    Dial_Stop();
    Dial_SaveConfig();
}


/*******************************************************************************
* Function Name: Dial_Wakeup
********************************************************************************
*
* Summary:
*  Prepare Quadrature Decoder Component to wake up.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Dial_backup - used when non-retention registers are restored.
*
*******************************************************************************/
void Dial_Wakeup(void) 
{
    Dial_RestoreConfig();

    if (Dial_backup.enableState != 0u)
    {
        #if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT)
            Dial_Cnt8_Enable();
        #else 
            /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT) || 
            *  (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT) 
            */
            Dial_Cnt16_Enable();
        #endif /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT) */

        /* Enable component's operation */
        Dial_Enable();
    } /* Do nothing if component's block was disabled before */
}


/* [] END OF FILE */

